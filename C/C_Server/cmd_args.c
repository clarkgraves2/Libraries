/** @file cmd_args.c
 *
 * @brief Implementation of command line argument processing.
 *
 * This module processes and validates command line arguments for
 * configuring the chat server.
 */

 #include <stdbool.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <getopt.h>
 #include "cmd_args.h"
 
 /* Print usage information */
 static void print_usage(const char *program_name) 
 {
     printf("Usage: %s [OPTIONS]\n", program_name);
     printf("Options:\n");
     printf("  -p, --port PORT           Server port number (default: 8080)\n");
     printf("  -d, --db-path PATH        Path to user database file\n");
     printf("  -t, --threads COUNT       Number of worker threads (default: 4)\n");
     printf("  -v, --verbose             Enable verbose logging\n");
     printf("  -h, --help                Display this help message\n");
 }
 
 bool cmd_args_validation(int argc, char *argv[], server_config_t *config) 
 {
     if (config == NULL) 
     {
         return false;
     }
     
     /* Default values */
     config->port = 8080;
     config->db_path = "users.db";
     config->catalog_path = "catalog.db";
     config->thread_count = 4;
     config->verbose_logging = false;
     
     /* Define long options */
     static struct option long_options[] = {
         {"port",          required_argument, 0, 'p'},
         {"db-path",       required_argument, 0, 'd'},
         {"catalog-path",  required_argument, 0, 'c'},
         {"threads",       required_argument, 0, 't'},
         {"verbose",       no_argument,       0, 'v'},
         {"help",          no_argument,       0, 'h'},
         {0, 0, 0, 0}
     };
     
     int opt;
     int long_index = 0;
     
     /* Reset getopt */
     optind = 0;
     
     while ((opt = getopt_long(argc, argv, "p:d:c:t:vh", 
                              long_options, &long_index)) != -1) 
     {
         switch (opt) 
         {
             case 'p':
                 config->port = atoi(optarg);
                 if (config->port <= 0 || config->port > 65535) 
                 {
                     fprintf(stderr, "Error: Invalid port number\n");
                     return false;
                 }
                 break;
                 
             case 'd':
                 config->db_path = optarg;
                 break;
                 
             case 'c':
                 config->catalog_path = optarg;
                 break;
                 
             case 't':
                 config->thread_count = atoi(optarg);
                 if (config->thread_count <= 0) 
                 {
                     fprintf(stderr, "Error: Invalid thread count\n");
                     return false;
                 }
                 break;
                 
             case 'v':
                 config->verbose_logging = true;
                 break;
                 
             case 'h':
                 print_usage(argv[0]);
                 exit(EXIT_SUCCESS);
                 break;
                 
             default:
                 print_usage(argv[0]);
                 return false;
         }
     }
     
     return true;
 }