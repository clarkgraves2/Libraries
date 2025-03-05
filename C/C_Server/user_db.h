/** @file user_db.h
 *
 * @brief Public interface for user database management.
 *
 * This module provides functions to manage users in a database,
 * including creating, authenticating, and retrieving user information.
 * It handles persistence and secure storage of user credentials.
 */

 #ifndef USER_DB_H
 #define USER_DB_H
 
 #include <stdint.h>
 #include <stdbool.h>
 #include <time.h>
 
 /*************************************************************************
  * Constants
  *************************************************************************/
 
 #define USER_DB_MAX_USERS              (100)  
 #define USER_DB_MAX_USERNAME_LEN       (32)   
 #define USER_DB_MAX_PASSWORD_LEN       (64)   
 #define USER_DB_MIN_PASSWORD_LEN       (8)    
 #define USER_DB_MAX_LOGIN_ATTEMPTS     (5)  
 #define USER_DB_PASSWORD_SALT_LEN      (16)
 #define USER_DB_LOCK_DURATION_MINS     (30)
 
 /*************************************************************************
  * Type Definitions
  *************************************************************************/
 
 /* User permission levels */
 typedef enum 
 {
     USER_DB_GUEST = 0,    /* Limited access permissions */
     USER_DB_USER,         /* Standard user permissions */
     USER_DB_ADMIN,        /* Administrative permissions */
     USER_DB_COUNT       
 } user_db_role_t;
 
 /* Data Action/Status Codes */
 typedef enum 
 {
     USER_DB_SUCCESS = 0,        /* Operation completed successfully */
     USER_DB_NOT_INITIALIZED,    /* Database not initialized */
     USER_DB_INVALID_PARAM,      /* Invalid parameter provided */
     USER_DB_NOT_FOUND,          /* User not found */
     USER_DB_ALREADY_EXISTS,     /* User already exists */
     USER_DB_DB_FULL,            /* User database is full */
     USER_DB_INVALID_CREDENTIALS,/* Invalid username or password */
     USER_DB_ACCOUNT_LOCKED,     /* Account is locked due to failed attempts */
     USER_DB_INVALID_DATA,       /* Invalid user data */
 } user_db_status_t;
 
 /* Public user information structure */
 typedef struct 
 {
     uint32_t id;                                 /* Unique user identifier */
     char username[USER_DB_MAX_USERNAME_LEN];     /* User's login name */
     char password[USER_DB_MAX_PASSWORD_LEN];     /* User's password */
     user_db_role_t role;                         /* User's permission */
 } user_db_user_record_t;
 
 /* Internal user structure with more fields */
 typedef struct 
 {
     uint32_t        id;                                  /* Unique user ID */
     char            username[USER_DB_MAX_USERNAME_LEN];  /* Username */
     char            password_hash[64];                   /* Password hash */
     char            salt[USER_DB_PASSWORD_SALT_LEN];     /* Salt for password */
     user_db_role_t  role;                               /* User role/permissions */
     uint32_t        login_attempts;                     /* Failed login counter */
     time_t          lockout_time;                       /* Time of account lockout */
     bool            b_active;                           /* Is account active */
 } user_db_user_t;
 
 /*************************************************************************
  * Function Prototypes
  *************************************************************************/
 
 /**
  * @brief Initialize the user database
  *
  * Loads user data from persistence storage if available.
  * Must be called before any other user_db functions.
  *
  * @param[in] p_db_path Path to the database file (NULL for default)
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_init(const char* p_db_path);
 
 /**
  * @brief Clean up and close the user database
  *
  * Ensures all data is saved and releases resources.
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_cleanup(void);
 
 /**
  * @brief Add a new user to the database
  *
  * @param[in]  p_username  Username for the new user
  * @param[in]  p_password  Password for the new user (will be securely hashed)
  * @param[in]  role        Permission role for the new user
  * @param[out] p_user_id   Pointer to store the new user's ID if successful
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_add_user(const char* p_username, 
                              const char* p_password,
                              user_db_role_t role,
                              uint32_t* p_user_id);
 
 /**
  * @brief Authenticate a user with username and password
  *
  * @param[in]  p_username  Username to authenticate
  * @param[in]  p_password  Password to verify
  * @param[out] p_user_id   Pointer to store user ID if authentication succeeds
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_authenticate(const char* p_username,
                                       const char* p_password,
                                       uint32_t* p_user_id);
 
 /**
  * @brief Get information about a user by ID
  *
  * @param[in]  user_id   ID of user to retrieve
  * @param[out] p_record  Pointer to store user information
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_get_user_info(uint32_t user_id, user_db_user_record_t* p_record);
 
 /**
  * @brief Find a user by username
  *
  * @param[in]  p_username  Username to search for
  * @param[out] p_user_id   Pointer to store user ID if found
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_find_by_username(const char* p_username, 
                                           uint32_t* p_user_id);
 
 /**
  * @brief Update user information
  *
  * @param[in] user_id   ID of user to update
  * @param[in] p_record  Updated user information
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_update(uint32_t user_id, const user_db_user_record_t* p_record);
 
 /**
  * @brief Change a user's password
  *
  * @param[in] user_id          ID of user
  * @param[in] p_old_password   Current password (for verification)
  * @param[in] p_new_password   New password to set
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_change_password(uint32_t user_id,
                                         const char* p_old_password,
                                         const char* p_new_password);
 
 /**
  * @brief Delete a user from the database
  *
  * @param[in] user_id   ID of user to delete
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_delete_user(uint32_t user_id);
 
 /**
  * @brief List all users in the database
  *
  * @param[out] p_users     Array to store user information
  * @param[in]  max_users   Maximum number of users to retrieve
  * @param[out] p_count     Number of users actually retrieved
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_list(user_db_user_record_t* p_users, 
                              uint32_t max_users,
                              uint32_t* p_count);
 
 /**
  * @brief Force save of database to persistent storage
  *
  * @return Status code indicating success or error
  */
 user_db_status_t user_db_save(void);
 
 #endif /* USER_DB_H */
 
 /*** end of file ***/