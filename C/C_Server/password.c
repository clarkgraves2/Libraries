/** @file password.c
 *
 * @brief Implementation of secure password handling functions.
 *
 * Provides secure password handling functionality including 
 * salting, hashing, and verification of passwords.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <stdint.h>
 
 #include "password.h"
 #include "syslog.h"
 
 /*************************************************************************
  * Static Function Prototypes
  *************************************************************************/
 
 static char* simple_sha256(const char* p_input, size_t len);
 
 /*************************************************************************
  * Public Functions
  *************************************************************************/
 
 /**
  * @brief Generate a random salt for password hashing
  *
  * @param[out] p_salt_out  Buffer to store generated salt
  * @param[in]  salt_len    Length of salt to generate
  *
  * @return true if successful, false otherwise
  */
 bool 
 password_generate_salt(char* p_salt_out, size_t salt_len)
 {
     if (NULL == p_salt_out || salt_len == 0)
     {
         return false;
     }
     
     /* Use a simple approach for generating salt */
     /* In a production system, use a cryptographically secure random function */
     srand((unsigned int)time(NULL));
     
     for (size_t i = 0; i < salt_len; i++)
     {
         /* Generate printable ASCII characters (range 33-126) */
         p_salt_out[i] = (char)(rand() % 94 + 33);
     }
     
     return true;
 }
 
 /**
  * @brief Set a user's password (generates salt and hash)
  *
  * @param[in]  p_password  Password to hash
  * @param[out] p_salt_out  Buffer to store salt
  * @param[out] p_hash_out  Buffer to store resulting hash
  *
  * @return true if successful, false otherwise
  */
 bool 
 password_set(const char* p_password, char* p_salt_out, char* p_hash_out)
 {
     if (NULL == p_password || NULL == p_salt_out || NULL == p_hash_out)
     {
         return false;
     }
     
     /* Generate new salt */
     if (!password_generate_salt(p_salt_out, USER_DB_PASSWORD_SALT_LEN))
     {
         return false;
     }
     
     /* Hash the password with the new salt */
     return password_hash(p_password, p_salt_out, p_hash_out);
 }
 
 /**
  * @brief Verify a password against stored hash and salt
  *
  * @param[in] p_password  Password to verify
  * @param[in] p_salt      Salt to use for verification
  * @param[in] p_hash      Hash to compare against
  *
  * @return true if password matches, false otherwise
  */
 bool 
 password_verify(const char* p_password, const char* p_salt, const char* p_hash)
 {
     if (NULL == p_password || NULL == p_salt || NULL == p_hash)
     {
         return false;
     }
     
     /* Hash the provided password with the stored salt */
     char computed_hash[PASSWORD_HASH_SIZE];
     if (!password_hash(p_password, p_salt, computed_hash))
     {
         return false;
     }
     
     /* Compare the calculated hash with the stored hash */
     return (0 == strcmp(computed_hash, p_hash));
 }
 
 /**
  * @brief Hash a password with salt
  *
  * @param[in]  p_password  Password to hash
  * @param[in]  p_salt      Salt to use for hashing
  * @param[out] p_hash_out  Buffer to store resulting hash
  *
  * @return true if successful, false otherwise
  * 
  * @note This is a simple password hashing implementation for demonstration.
  *       In a production system, use a proper cryptographic library with
  *       secure algorithms like bcrypt, scrypt, or PBKDF2.
  */
 bool 
 password_hash(const char* p_password, const char* p_salt, char* p_hash_out)
 {
     if (NULL == p_password || NULL == p_salt || NULL == p_hash_out)
     {
         return false;
     }
     
     /* Create salted password (password + salt) */
     char salted_password[USER_DB_MAX_PASSWORD_LEN + USER_DB_PASSWORD_SALT_LEN];
     
     /* Copy password */
     size_t password_len = strlen(p_password);
     if (password_len >= USER_DB_MAX_PASSWORD_LEN)
     {
         return false;
     }
     
     memcpy(salted_password, p_password, password_len);
     
     /* Append salt */
     size_t salt_len = strlen(p_salt);
     if (salt_len > USER_DB_PASSWORD_SALT_LEN)
     {
         return false;
     }
     
     memcpy(salted_password + password_len, p_salt, salt_len);
     
     /* Calculate hash of salted password */
     /* In a production system, use a secure cryptographic library */
     char* hash = simple_sha256(salted_password, password_len + salt_len);
     if (NULL == hash)
     {
         return false;
     }
     
     /* Copy hash to output buffer */
     strcpy(p_hash_out, hash);
     
     /* Free temporary hash buffer */
     free(hash);
     
     return true;
 }
 
 /*************************************************************************
  * Static Functions
  *************************************************************************/
 
 /**
  * @brief Simple SHA-256 hash implementation
  *
  * @param[in] p_input  Input data to hash
  * @param[in] len      Length of input data
  *
  * @return Pointer to hash string (caller must free), or NULL on error
  *
  * @note This is a simplified placeholder implementation.
  *       In a production system, use a proper cryptographic library.
  */
 static char*
 simple_sha256(const char* p_input, size_t len)
 {
     /* NOTE: This is a placeholder implementation for demonstration purposes! */
     /* In a real implementation, use a proper cryptographic library like OpenSSL */
     
     if (NULL == p_input || len == 0)
     {
         return NULL;
     }
     
     /* Allocate buffer for hash output (64 chars for hex representation of SHA-256) */
     char* hash = (char*)malloc(PASSWORD_HASH_SIZE);
     if (NULL == hash)
     {
         return NULL;
     }
     
     /* Simple hash algorithm - NOT SECURE, just for demonstration */
     /* XOR and rotate bits based on input */
     uint8_t result[32] = {0}; /* 256 bits = 32 bytes */
     
     for (size_t i = 0; i < len; i++)
     {
         uint8_t byte = (uint8_t)p_input[i];
         result[i % 32] ^= byte;
         
         /* Rotate the result array */
         uint8_t temp = result[0];
         for (int j = 0; j < 31; j++)
         {
             result[j] = result[j+1];
         }
         result[31] = temp;
     }
     
     /* Convert to hex string */
     for (int i = 0; i < 32; i++)
     {
         sprintf(hash + (i * 2), "%02x", result[i]);
     }
     
     return hash;
 }
 
 /*** end of file ***/