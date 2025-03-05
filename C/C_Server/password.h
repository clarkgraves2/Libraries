/** @file password.h
 *
 * @brief Interface for secure password handling functions.
 *
 * This module provides functions for securely hashing, salting,
 * and verifying passwords.
 */

 #ifndef PASSWORD_H
 #define PASSWORD_H
 
 #include <stdbool.h>
 #include <stddef.h>
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 #define PASSWORD_HASH_SIZE       (64)    /* SHA-256 hash size in chars */
 
 /*************************************************************************
  * Function Prototypes
  *************************************************************************/
 
 /**
  * @brief Generate a random salt for password hashing
  *
  * @param[out] p_salt_out  Buffer to store generated salt
  * @param[in]  salt_len    Length of salt to generate
  *
  * @return true if successful, false otherwise
  */
 bool password_generate_salt(char* p_salt_out, size_t salt_len);
 
 /**
  * @brief Set a user's password (generates salt and hash)
  *
  * @param[in]  p_password  Password to hash
  * @param[out] p_salt_out  Buffer to store salt
  * @param[out] p_hash_out  Buffer to store resulting hash
  *
  * @return true if successful, false otherwise
  */
 bool password_set(const char* p_password, char* p_salt_out, char* p_hash_out);
 
 /**
  * @brief Verify a password against stored hash and salt
  *
  * @param[in] p_password  Password to verify
  * @param[in] p_salt      Salt to use for verification
  * @param[in] p_hash      Hash to compare against
  *
  * @return true if password matches, false otherwise
  */
 bool password_verify(const char* p_password, const char* p_salt, const char* p_hash);
 
 /**
  * @brief Hash a password with salt
  *
  * @param[in]  p_password  Password to hash
  * @param[in]  p_salt      Salt to use for hashing
  * @param[out] p_hash_out  Buffer to store resulting hash
  *
  * @return true if successful, false otherwise
  */
 bool password_hash(const char* p_password, const char* p_salt, char* p_hash_out);
 
 #endif /* PASSWORD_H */
 
 /*** end of file ***/