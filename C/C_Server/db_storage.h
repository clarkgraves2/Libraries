/** @file db_storage.h
 *
 * @brief Interface for database file storage operations.
 *
 * This module provides functions for saving and loading user database
 * information to/from persistent storage.
 */

 #ifndef DB_STORAGE_H
 #define DB_STORAGE_H
 
 #include <stdbool.h>
 #include <stdint.h>
 #include "user_db.h"
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 #define DB_STORAGE_FILE_HEADER  "USER_DB_V1.0"
 #define DB_STORAGE_HEADER_LEN   (12)
 
 /*************************************************************************
  * Function Prototypes
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
 bool db_storage_save(const char* p_db_path, 
                    const user_db_user_t* p_users, 
                    uint32_t user_count,
                    uint32_t next_user_id);
 
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
 bool db_storage_load(const char* p_db_path, 
                    user_db_user_t* p_users,
                    uint32_t* p_user_count,
                    uint32_t* p_next_user_id);
 
 #endif /* DB_STORAGE_H */
 
 /*** end of file ***/