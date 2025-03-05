/** @file user_db.c
 *
 * @brief Core user database management functionality.
 *
 * This module implements the core database operations for managing users,
 * including adding, finding, and updating user records.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdint.h>
 #include <time.h>
 #include <pthread.h>
 
 #include "user_db.h"
 #include "password.h"
 #include "db_storage.h"
 #include "syslog.h"
 #include "cleanup.h"
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 #define USER_DB_DEFAULT_PATH "users.db"
 
 /*************************************************************************
  * Static Variables
  *************************************************************************/
 
 /* The user database array */
 static user_db_user_t g_users[USER_DB_MAX_USERS];
 
 /* Current number of users in the database */
 static uint32_t g_user_count = 0;
 
 /* Next available user ID */
 static uint32_t g_next_user_id = 1;
 
 /* Path to the database file */
 static char g_db_path[256] = {0};
 
 /* Database initialization flag */
 static bool g_b_initialized = false;
 
 /* Mutex for thread-safety */
 static pthread_mutex_t g_db_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 /*************************************************************************
  * Static Function Prototypes
  *************************************************************************/
 
 static uint32_t user_db_find_by_username_internal(const char* p_username);
 static int cleanup_user_db(void);
 static bool is_account_locked(const user_db_user_t* p_user);
 
 /*************************************************************************
  * Public Functions
  *************************************************************************/
 
 /**
  * @brief Initialize the user database
  *
  * @param[in] p_db_path Path to the database file (NULL for default)
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_init(const char* p_db_path)
 {
     pthread_mutex_lock(&g_db_mutex);
     
     /* Check if already initialized */
     if (g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_SUCCESS;
     }
     
     /* Clear the user database */
     memset(g_users, 0, sizeof(g_users));
     g_user_count = 0;
     g_next_user_id = 1;
     
     /* Set database path (use default if NULL) */
     if (NULL == p_db_path)
     {
         strncpy(g_db_path, USER_DB_DEFAULT_PATH, sizeof(g_db_path) - 1);
     }
     else
     {
         strncpy(g_db_path, p_db_path, sizeof(g_db_path) - 1);
     }
     
     /* Load database from file */
     if (!db_storage_load(g_db_path, g_users, &g_user_count, &g_next_user_id))
     {
         syslog_write(WARNING, SYSLOG_DEST_NONE, 
                    "Failed to load user database from %s. Creating new database.", 
                    g_db_path);
     }
     
     /* Register cleanup function */
     cleanup_add_int(cleanup_user_db, NULL, 10);
     
     g_b_initialized = true;
     
     pthread_mutex_unlock(&g_db_mutex);
     return USER_DB_SUCCESS;
 }
 
 /**
  * @brief Clean up and close the user database
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_cleanup(void)
 {
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Save database to file */
     user_db_status_t status = USER_DB_SUCCESS;
     
     if (!db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id))
     {
         status = USER_DB_INVALID_DATA;
     }
     
     g_b_initialized = false;
     
     pthread_mutex_unlock(&g_db_mutex);
     return status;
 }
 
 /**
  * @brief Add a new user to the database
  *
  * @param[in]  p_username  Username for the new user
  * @param[in]  p_password  Password for the new user
  * @param[in]  role        Permission role for the new user
  * @param[out] p_user_id   Pointer to store the new user's ID if successful
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_add_user(const char* p_username, 
                 const char* p_password,
                 user_db_role_t role,
                 uint32_t* p_user_id)
 {
     /* Check inputs */
     if (NULL == p_username || NULL == p_password || NULL == p_user_id)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     if (role >= USER_DB_COUNT)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     if (strlen(p_username) == 0 || strlen(p_username) >= USER_DB_MAX_USERNAME_LEN)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     if (strlen(p_password) < USER_DB_MIN_PASSWORD_LEN || 
         strlen(p_password) >= USER_DB_MAX_PASSWORD_LEN)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Check if username already exists */
     if (user_db_find_by_username_internal(p_username) != 0)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_ALREADY_EXISTS;
     }
     
     /* Check if database is full */
     if (g_user_count >= USER_DB_MAX_USERS)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_DB_FULL;
     }
     
     /* Find an empty slot for the new user */
     uint32_t new_user_index = g_user_count;
     
     /* Fill in user data */
     user_db_user_t* p_new_user = &g_users[new_user_index];
     p_new_user->id = g_next_user_id++;
     strncpy(p_new_user->username, p_username, USER_DB_MAX_USERNAME_LEN - 1);
     p_new_user->username[USER_DB_MAX_USERNAME_LEN - 1] = '\0';
     
     /* Set up password */
     if (!password_set(p_password, p_new_user->salt, p_new_user->password_hash))
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_INVALID_DATA;
     }
     
     p_new_user->role = role;
     p_new_user->login_attempts = 0;
     p_new_user->lockout_time = 0;
     p_new_user->b_active = true;
     
     g_user_count++;
     *p_user_id = p_new_user->id;
     
     /* Save changes to file */
     user_db_status_t status = USER_DB_SUCCESS;
     
     if (!db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id))
     {
         status = USER_DB_INVALID_DATA;
     }
     
     pthread_mutex_unlock(&g_db_mutex);
     return status;
 }
 
 /**
  * @brief Authenticate a user with username and password
  *
  * @param[in]  p_username  Username to authenticate
  * @param[in]  p_password  Password to verify
  * @param[out] p_user_id   Pointer to store user ID if authentication succeeds
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_authenticate(const char* p_username,
                    const char* p_password,
                    uint32_t* p_user_id)
 {
     /* Check inputs */
     if (NULL == p_username || NULL == p_password || NULL == p_user_id)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Find user by username */
     uint32_t user_index = user_db_find_by_username_internal(p_username);
     if (user_index == 0)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_FOUND;
     }
     
     user_db_user_t* p_user = &g_users[user_index - 1];
     
     /* Check if account is locked */
     if (is_account_locked(p_user))
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_ACCOUNT_LOCKED;
     }
     
     /* Verify password */
     if (!password_verify(p_password, p_user->salt, p_user->password_hash))
     {
         /* Increment failed login attempts */
         p_user->login_attempts++;
         
         /* Lock account if too many failed attempts */
         if (p_user->login_attempts >= USER_DB_MAX_LOGIN_ATTEMPTS)
         {
             p_user->lockout_time = time(NULL);
             syslog_write(WARNING, SYSLOG_DEST_NONE, 
                        "Account '%s' locked due to too many failed login attempts",
                        p_username);
         }
         
         /* Save changes */
         db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id);
         
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_INVALID_CREDENTIALS;
     }
     
     /* Authentication successful, reset failed login attempts */
     p_user->login_attempts = 0;
     *p_user_id = p_user->id;
     
     /* Save changes */
     db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id);
     
     pthread_mutex_unlock(&g_db_mutex);
     return USER_DB_SUCCESS;
 }
 
 /**
  * @brief Get information about a user by ID
  *
  * @param[in]  user_id   ID of user to retrieve
  * @param[out] p_record  Pointer to store user information
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_get_user_info(uint32_t user_id, user_db_user_record_t* p_record)
 {
     /* Check inputs */
     if (NULL == p_record)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Find user by ID */
     bool b_found = false;
     
     for (uint32_t i = 0; i < g_user_count; i++)
     {
         if (g_users[i].id == user_id && g_users[i].b_active)
         {
             /* Copy user data to output record */
             p_record->id = g_users[i].id;
             strncpy(p_record->username, g_users[i].username, USER_DB_MAX_USERNAME_LEN);
             p_record->username[USER_DB_MAX_USERNAME_LEN - 1] = '\0';
             
             /* Set password field to empty (don't expose the hash) */
             p_record->password[0] = '\0';
             
             p_record->role = g_users[i].role;
             
             b_found = true;
             break;
         }
     }
     
     pthread_mutex_unlock(&g_db_mutex);
     
     if (!b_found)
     {
         return USER_DB_NOT_FOUND;
     }
     
     return USER_DB_SUCCESS;
 }
 
 /**
  * @brief Find a user by username
  *
  * @param[in]  p_username  Username to search for
  * @param[out] p_user_id   Pointer to store user ID if found
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_find_by_username(const char* p_username, uint32_t* p_user_id)
 {
     /* Check inputs */
     if (NULL == p_username || NULL == p_user_id)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Find user by username */
     uint32_t user_index = user_db_find_by_username_internal(p_username);
     
     pthread_mutex_unlock(&g_db_mutex);
     
     if (user_index == 0)
     {
         return USER_DB_NOT_FOUND;
     }
     
     *p_user_id = g_users[user_index - 1].id;
     return USER_DB_SUCCESS;
 }
 
 /**
  * @brief Update user information
  *
  * @param[in] user_id   ID of user to update
  * @param[in] p_record  Updated user information
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_update(uint32_t user_id, const user_db_user_record_t* p_record)
 {
     /* Check inputs */
     if (NULL == p_record)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     if (p_record->role >= USER_DB_COUNT)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Find user by ID */
     int user_index = -1;
     
     for (uint32_t i = 0; i < g_user_count; i++)
     {
         if (g_users[i].id == user_id && g_users[i].b_active)
         {
             user_index = i;
             break;
         }
     }
     
     if (user_index == -1)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_FOUND;
     }
     
     /* Check if username is being changed and if it already exists */
     if (0 != strcmp(g_users[user_index].username, p_record->username))
     {
         uint32_t existing_user = user_db_find_by_username_internal(p_record->username);
         if (existing_user != 0 && existing_user - 1 != (uint32_t)user_index)
         {
             pthread_mutex_unlock(&g_db_mutex);
             return USER_DB_ALREADY_EXISTS;
         }
     }
     
     /* Update user data */
     strncpy(g_users[user_index].username, p_record->username, USER_DB_MAX_USERNAME_LEN - 1);
     g_users[user_index].username[USER_DB_MAX_USERNAME_LEN - 1] = '\0';
     
     /* If password field is not empty, update password */
     if (p_record->password[0] != '\0')
     {
         if (!password_set(p_record->password, 
                         g_users[user_index].salt, 
                         g_users[user_index].password_hash))
         {
             pthread_mutex_unlock(&g_db_mutex);
             return USER_DB_INVALID_DATA;
         }
     }
     
     g_users[user_index].role = p_record->role;
     
     /* Save changes to file */
     user_db_status_t status = USER_DB_SUCCESS;
     
     if (!db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id))
     {
         status = USER_DB_INVALID_DATA;
     }
     
     pthread_mutex_unlock(&g_db_mutex);
     return status;
 }
 
 /**
  * @brief Change a user's password
  *
  * @param[in] user_id          ID of user
  * @param[in] p_old_password   Current password (for verification)
  * @param[in] p_new_password   New password to set
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_change_password(uint32_t user_id,
                       const char* p_old_password,
                       const char* p_new_password)
 {
     /* Check inputs */
     if (NULL == p_old_password || NULL == p_new_password)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     if (strlen(p_new_password) < USER_DB_MIN_PASSWORD_LEN || 
         strlen(p_new_password) >= USER_DB_MAX_PASSWORD_LEN)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Find user by ID */
     int user_index = -1;
     
     for (uint32_t i = 0; i < g_user_count; i++)
     {
         if (g_users[i].id == user_id && g_users[i].b_active)
         {
             user_index = i;
             break;
         }
     }
     
     if (user_index == -1)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_FOUND;
     }
     
     /* Verify old password */
     if (!password_verify(p_old_password, 
                        g_users[user_index].salt, 
                        g_users[user_index].password_hash))
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_INVALID_CREDENTIALS;
     }
     
     /* Update password */
     if (!password_set(p_new_password, 
                     g_users[user_index].salt, 
                     g_users[user_index].password_hash))
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_INVALID_DATA;
     }
     
     /* Save changes to file */
     user_db_status_t status = USER_DB_SUCCESS;
     
     if (!db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id))
     {
         status = USER_DB_INVALID_DATA;
     }
     
     pthread_mutex_unlock(&g_db_mutex);
     return status;
 }
 
 /**
  * @brief Delete a user from the database
  *
  * @param[in] user_id   ID of user to delete
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_delete_user(uint32_t user_id)
 {
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Find user by ID */
     int user_index = -1;
     
     for (uint32_t i = 0; i < g_user_count; i++)
     {
         if (g_users[i].id == user_id && g_users[i].b_active)
         {
             user_index = i;
             break;
         }
     }
     
     if (user_index == -1)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_FOUND;
     }
     
     /* Mark user as inactive (soft delete) */
     g_users[user_index].b_active = false;
     
     /* Save changes to file */
     user_db_status_t status = USER_DB_SUCCESS;
     
     if (!db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id))
     {
         status = USER_DB_INVALID_DATA;
     }
     
     pthread_mutex_unlock(&g_db_mutex);
     return status;
 }
 
 /**
  * @brief List all users in the database
  *
  * @param[out] p_users     Array to store user information
  * @param[in]  max_users   Maximum number of users to retrieve
  * @param[out] p_count     Number of users actually retrieved
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_list(user_db_user_record_t* p_users, 
            uint32_t max_users,
            uint32_t* p_count)
 {
     /* Check inputs */
     if (NULL == p_users || NULL == p_count || max_users == 0)
     {
         return USER_DB_INVALID_PARAM;
     }
     
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     /* Count active users and copy to output array */
     uint32_t count = 0;
     
     for (uint32_t i = 0; i < g_user_count && count < max_users; i++)
     {
         if (g_users[i].b_active)
         {
             p_users[count].id = g_users[i].id;
             strncpy(p_users[count].username, g_users[i].username, USER_DB_MAX_USERNAME_LEN);
             p_users[count].username[USER_DB_MAX_USERNAME_LEN - 1] = '\0';
             
             /* Set password field to empty (don't expose the hash) */
             p_users[count].password[0] = '\0';
             
             p_users[count].role = g_users[i].role;
             
             count++;
         }
     }
     
     *p_count = count;
     
     pthread_mutex_unlock(&g_db_mutex);
     return USER_DB_SUCCESS;
 }
 
 /**
  * @brief Force save of database to persistent storage
  *
  * @return Status code indicating success or error
  */
 user_db_status_t 
 user_db_save(void)
 {
     pthread_mutex_lock(&g_db_mutex);
     
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_db_mutex);
         return USER_DB_NOT_INITIALIZED;
     }
     
     user_db_status_t status = USER_DB_SUCCESS;
     
     if (!db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id))
     {
         status = USER_DB_INVALID_DATA;
     }
     
     pthread_mutex_unlock(&g_db_mutex);
     return status;
 }
 
 /*************************************************************************
  * Static Functions
  *************************************************************************/
 
 /**
  * @brief Cleanup function registered with the cleanup system
  *
  * @return 0 on success, negative value on error
  */
 static int
 cleanup_user_db(void)
 {
     pthread_mutex_lock(&g_db_mutex);
     
     if (g_b_initialized)
     {
         db_storage_save(g_db_path, g_users, g_user_count, g_next_user_id);
         g_b_initialized = false;
     }
     
     pthread_mutex_unlock(&g_db_mutex);
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "User database cleanup complete");
     return 0;
 }
 
 /**
  * @brief Check if an account is locked due to too many failed login attempts
  *
  * @param[in] p_user  Pointer to user entry
  *
  * @return true if account is locked, false otherwise
  */
 static bool
 is_account_locked(const user_db_user_t* p_user)
 {
     if (NULL == p_user)
     {
         return false;
     }
     
     /* Account is locked if lockout time is set and lockout period hasn't expired */
     if (p_user->lockout_time > 0)
     {
         time_t now = time(NULL);
         time_t lockout_end = p_user->lockout_time + (USER_DB_LOCK_DURATION_MINS * 60);
         
         if (now < lockout_end)
         {
             return true;
         }
     }
     
     return false;
 }
 
 /**
  * @brief Internal function to find a user by username
  *
  * @param[in] p_username  Username to search for
  *
  * @return Index+1 of the user in the array, 0 if not found
  * @note Returns index+1 so that 0 can be used to indicate "not found"
  * @note Assumes mutex is already locked
  */
 static uint32_t 
 user_db_find_by_username_internal(const char* p_username)
 {
     if (NULL == p_username)
     {
         return 0;
     }
     
     for (uint32_t i = 0; i < g_user_count; i++)
     {
         if (g_users[i].b_active && 
             strcmp(g_users[i].username, p_username) == 0)
         {
             return i + 1;  /* Return index+1 (0 means not found) */
         }
     }
     
     return 0;
 }
 
 /*** end of file ***/