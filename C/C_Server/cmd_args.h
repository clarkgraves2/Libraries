// args.h
#ifndef CMD_ARGS_H
#define CMD_ARGS_H

/**
 * Server configuration parameters parsed from command-line arguments
 */
typedef struct 
{
    int port;                   // Server listening port
    const char *db_path;        // Path to user database
    const char *catalog_path;   // Path to book catalog
    int thread_count;           // Number of worker threads
    bool verbose_logging;       // Enable verbose logging
} server_config_t;

/**
 * Parse and validate command line arguments
 *
 * @param argc Argument count from main
 * @param argv Argument array from main
 * @param config Output configuration structure
 * @return true if arguments valid, false otherwise
 */
bool cmd_args_validation(int argc, char *argv[], server_config_t *config);

#endif /* CMD_ARGS_H */