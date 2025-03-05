/** @file db_storage.c
 *
 * @brief Implementation of database file storage operations.
 *
 * Provides functions to save and load user database information
 * to and from persistent storage with proper error handling.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <errno.h>
 #include <unistd.h>
 #include <sys/stat.h>
 
 #include "db_storage.h"
 #include "user_db.h"
 #include "syslog.h"
 
 /*************************************************************************
  * Public Functions
  *************************************************************************/
 
 /**
  * @brief Save user database to file
  *
  * @param[in] p_db_path     Path to the database file
  * @param[in] p_users       Array of user records
  * @param[in] user_count    Number of users in the array
  * @param[in] next_user_id  Next available user ID
  *
  * @return true if successful, false otherwise
  */
 bool 
 db_storage_save(const char* p_db_path, 
               const user_db_user_t* p_users, 
               uint32_t user_count,
               uint32_t next_user_id)
 {
     /* Validate input parameters */
     if (NULL == p_db_path || NULL == p_users)
     {
         return false;
     }
     
     /* Create a temporary file for writing */
     char temp_path[260];
     snprintf(temp_path, sizeof(temp_path), "%s.tmp", p_db_path);
     
     FILE* p_file = fopen(temp_path, "wb");
     if (NULL == p_file)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                    "Failed to open user database file for writing: %s", 
                    strerror(errno));
         return false;
     }
     
     /* Write file header */
     if (fwrite(DB_STORAGE_FILE_HEADER, 1, DB_STORAGE_HEADER_LEN, p_file) != DB_STORAGE_HEADER_LEN)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                    "Failed to write user database header: %s", 
                    strerror(errno));
         fclose(p_file);
         unlink(temp_path);
         return false;
     }
     
     /* Write user count */
     if (fwrite(&user_count, sizeof(user_count), 1, p_file) != 1)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                    "Failed to write user count: %s", 
                    strerror(errno));
         fclose(p_file);
         unlink(temp_path);
         return false;
     }
     
     /* Write next user ID */
     if (fwrite(&next_user_id, sizeof(next_user_id), 1, p_file) != 1)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                    "Failed to write next user ID: %s", 
                    strerror(errno));
         fclose(p_file);
         unlink(temp_path);
         return false;
     }
     
     /* Write user entries */
     if (fwrite(p_users, sizeof(user_db_user_t), user_count, p_file) != user_count)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                    "Failed to write user entries: %s", 
                    strerror(errno));
         fclose(p_file);
         unlink(temp_path);
         return false;
     }
     
     /* Flush and close the file */
     fflush(p_file);
     fclose(p_file);
     
     /* Atomically replace the old file with the new one */
     if (rename(temp_path, p_db_path) != 0)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                    "Failed to rename temp database file: %s", 
                    strerror(errno));
         unlink(temp_path);
         return false;
     }
     
     /* Set permissions to read/write for owner only */
     chmod(p_db_path, S_IRUSR | S_IWUSR);
     
     return true;
 }
 
 /**
  * @brief Load user database from file
  *
  * @param[in]  p_db_path      Path to the database file
  * @param[out] p_users        Array to store user records
  * @param[out] p_user_count   Pointer to store number of users
  * @param[out] p_next_user_id Pointer to store next available user ID
  *
  * @return true if successful, false otherwise
  */
 bool 
 db_storage_load(const char* p_db_path, 
               user_db_user_t* p_users,
               uint32_t* p_user_count,
               uint32_t* p_next_user_id)
 {
     /* Validate input parameters */
     if (NULL == p_db_path || NULL == p_users || 
         NULL == p_user_count || NULL == p_next_user_id)
     {
         return false;
     }
     
     FILE* p_file = fopen(p_db_path, "rb");
     if (NULL == p_file)
     {
         /* File doesn't exist or can't be opened - not an error for first run */
         return false;
     }
     
     /* Read and verify header */
     char header[DB_STORAGE_HEADER_LEN + 1];
     if (fread(header, 1, DB_STORAGE_HEADER_LEN, p_file) != DB_STORAGE_HEADER_LEN)
     {
         fclose(p_file);
         return false;
     }
     
     header[DB_STORAGE_HEADER_LEN] = '\0';
     
     if (strcmp(header, DB_STORAGE_FILE_HEADER) != 0)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "Invalid database file header: %s", header);
         fclose(p_file);
         return false;
     }
     
     /* Read user count */
     uint32_t user_count = 0;
     if (fread(&user_count, sizeof(user_count), 1, p_file) != 1)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "Failed to read user count from database");
         fclose(p_file);
         return false;
     }
     
     /* Read next user ID */
     uint32_t next_user_id = 0;
     if (fread(p_next_user_id, sizeof(next_user_id), 1, p_file) != 1)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "Failed to read next user ID from database");
         fclose(p_file);
         return false;
     }
     
     /* Check if user count is valid */
     if (user_count > USER_DB_MAX_USERS)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "User count in database exceeds maximum (%u > %u)", 
                     user_count, USER_DB_MAX_USERS);
         fclose(p_file);
         return false;
     }
     
     /* Read user entries */
     if (fread(p_users, sizeof(user_db_user_t), user_count, p_file) != user_count)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "Failed to read user entries from database");
         fclose(p_file);
         return false;
     }
     
     fclose(p_file);
     
     /* Set output parameters */
     *p_user_count = user_count;
     *p_next_user_id = next_user_id;
     
     syslog_write(INFO, SYSLOG_DEST_NONE, 
                 "Loaded %u users from database %s", 
                 user_count, p_db_path);
     
     return true;
 }
 
 /*** end of file ***/