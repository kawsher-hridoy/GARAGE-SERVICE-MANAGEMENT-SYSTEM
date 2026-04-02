/*
 * ===============================================================================
 * GARAGE Service MANAGEMENT SYSTEM (GSMS) - COMPLETE SINGLE FILE VERSION
 * ===============================================================================
 * 
 * University Project: Complete C Programming Implementation
 * 
 * Features:
 * - User Authentication System
 * - Customer & Vehicle Management  
 * - Service Management
 * - Billing & Invoice Generation
 * - Daily Revenue Reports
 * - Cross-Platform Compatibility
 * 
 * Originally developed as modular system, merged into single file
 * for university submission requirements.
 * 
 * Author: University Student
 * Course: C Programming
 * 
 * ===============================================================================
 */

/* ============================================================================
 * STANDARD LIBRARY INCLUDES
 * ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================================
 * PLATFORM-SPECIFIC INCLUDES AND MACROS
 * ============================================================================ */
#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(dir) mkdir(dir, 0755)
#endif

/* ============================================================================
 * CONSTANTS AND DEFINITIONS
 * ============================================================================ */

// System-wide constants
#define MAX_NAME_LENGTH 100
#define MAX_PHONE_LENGTH 20
#define MAX_EMAIL_LENGTH 100
#define MAX_ADDRESS_LENGTH 200

// Authentication constants
#define DEFAULT_USERNAME "admin"
#define DEFAULT_PASSWORD "admin"

// Service management constants
#define MAX_SERVICES 100

// Billing constants  
#define MAX_INVOICES 50

// Boolean type definition
typedef enum {
    false = 0,
    true = 1
} bool;

/* ============================================================================
 * ENUMERATIONS AND TYPE DEFINITIONS
 * ============================================================================ */

// ID Types for the ID manager
typedef enum {
    ID_CUSTOMER = 1,
    ID_VEHICLE = 2, 
    ID_SERVICE = 3,
    ID_PARTS = 4,
    ID_INVOICE = 5
} id_type_t;

// Customer structure (integrated with vehicle)
typedef struct {
    char id[10];              // Customer ID like "001"
    char name[50];            // Customer name
    char phone[20];           // Phone number  
    char address[100];        // Address
    
    // Vehicle information (integrated for simplicity)
    char vehicle_plate[20];   // License plate number like "ABC-123"
    char vehicle_make[30];    // Car brand like "Toyota"
    char vehicle_model[30];   // Car model like "Camry"
    char vehicle_year[5];     // Year like "2020"
    char vehicle_color[20];   // Color like "Red"
} customer_t;

// Service record structure
typedef struct {
    char service_id[20];      // Service ID
    int customer_id;          // Customer ID
    char service_name[100];   // Service name
    char date[20];            // Service date
    char status[20];          // "Pending", "In Progress", "Completed"
    double cost;              // Service cost
} Service;

// Invoice structure
typedef struct {
    char invoice_id[20];      // "INV001", "INV002", etc.
    char service_id[20];      // Links to service
    int customer_id;          // Links to customer
    char customer_name[100];  // Customer name
    char service_name[100];   // Service name
    char invoice_date[20];    // Date when invoice was generated
    double total_amount;      // Total to pay
    char payment_status[20];  // "Pending" or "Paid"
} Invoice;

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */

// Authentication system globals
bool user_logged_in = false;
char current_username[50] = "";

// Service management globals
static Service services[MAX_SERVICES];
static int service_count = 0;

// Billing system globals
static Invoice invoices[MAX_INVOICES];
static int invoice_count = 0;

// ID Manager globals
int customer_count = 0;
int vehicle_count = 1;
int service_count_id = 1;
int parts_count = 1;
int invoice_count_id = 1;

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

/**
 * Clear the screen (cross-platform)
 */
void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * Pause execution and wait for user input
 */
void pause_execution(void) {
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

/**
 * Clear input buffer to handle invalid input
 */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Safe string input function
 */
bool get_string_input(const char* prompt, char* buffer, int max_length) {
    printf("%s", prompt);
    
    if (fgets(buffer, max_length, stdin) == NULL) {
        return false;
    }
    
    // Find the newline and replace it with a null terminator
    buffer[strcspn(buffer, "\n")] = '\0';

    // Return true if the buffer is not empty
    return buffer[0] != '\0';
}

/**
 * Get integer input with validation
 */
bool get_int_input(const char* prompt, int* result) {
    char buffer[20];
    printf("%s", prompt);
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return false;
    }
    
    // Simple validation - check if it's a number
    char* endptr;
    long value = strtol(buffer, &endptr, 10);
    
    // Check if conversion was successful and within int range
    if (endptr != buffer && (*endptr == '\n' || *endptr == '\0')) {
        *result = (int)value;
        return true;
    }
    
    return false;
}

/**
 * Get double input with validation
 */
bool get_double_input(const char* prompt, double* result) {
    char buffer[20];
    printf("%s", prompt);
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return false;
    }
    
    char* endptr;
    *result = strtod(buffer, &endptr);
    
    // Check if conversion was successful
    return (endptr != buffer && (*endptr == '\n' || *endptr == '\0'));
}

/**
 * Check if string is empty or contains only whitespace
 */
bool is_empty_string(const char* str) {
    if (str == NULL) return true;
    
    while (*str != '\0') {
        if (*str != ' ' && *str != '\t' && *str != '\n' && *str != '\r') {
            return false;
        }
        str++;
    }
    
    return true;
}

/**
 * Simple year validation for vehicles
 */
bool validate_year(int year) {
    return (year >= 1900 && year <= 2026);
}

/**
 * Check if file exists
 */
bool file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

/**
 * Create data directory if it doesn't exist
 */
bool create_data_directory(void) {
    return (MKDIR("data") == 0 || file_exists("data"));
}

/**
 * Get current date in simple YYYY-MM-DD format
 */
void get_current_date(char* date_buffer) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(date_buffer, 20, "%Y-%m-%d", tm_info);
}

/* ============================================================================
 * AUTHENTICATION SYSTEM
 * ============================================================================ */

/**
 * Check if a user is currently logged in
 */
bool is_user_logged_in(void) {
    return user_logged_in;
}

/**
 * Validate username and password (simple hardcoded check)
 */
bool validate_credentials(const char* username, const char* password) {
    // Simple string comparison with default credentials
    if (strcmp(username, DEFAULT_USERNAME) == 0 && strcmp(password, DEFAULT_PASSWORD) == 0) {
        return true;
    }
    return false;
}

/**
 * Simple login function
 */
bool login_user(void) {
    char username[50];
    char password[50];
    
    // Clear screen and show professional login interface
    clear_screen();
    printf("\n");
    printf("  +==================================================================+\n");
    printf("  |                                                                  |\n");
    printf("  |                    \033[1mSYSTEM AUTHENTICATION\033[0m                    |\n");
    printf("  |                                                                  |\n");
    printf("  +==================================================================+\n");
    printf("\n");
    printf("  Please enter your credentials to access the system:\n");
    printf("  ----------------------------------------------------------\n");
    printf("\n");
    
    // Get username
    printf("  Username: ");
    if (!get_string_input("", username, sizeof(username))) {
        printf("  [X] Invalid username input.\n");
        pause_execution();
        return false;
    }
    
    // Get password
    printf("  Password: ");
    if (!get_string_input("", password, sizeof(password))) {
        printf("  [X] Invalid password input.\n");
        pause_execution();
        return false;
    }
    
    // Validate credentials
    if (validate_credentials(username, password)) {
        // Successful login
        user_logged_in = true;
        strcpy(current_username, username);
        
        printf("\n  [OK] Authentication successful!\n");
        printf("  Welcome, %s!\n", username);
        pause_execution();
        return true;
    } else {
        // Failed login
        printf("\n  [X] Authentication failed!\n");
        printf("  Invalid username or password.\n");
        pause_execution();
        return false;
    }
}

/**
 * Simple logout function
 */
void logout_user(void) {
    if (!is_user_logged_in()) {
        printf("  No user is currently logged in.\n");
        return;
    }
    
    clear_screen();
    printf("\n");
    printf("  +==================================================================+\n");
    printf("  |                                                                  |\n");
    printf("  |                        \033[1mSYSTEM LOGOUT\033[0m                        |\n");
    printf("  |                                                                  |\n");
    printf("  +==================================================================+\n");
    printf("\n");
    printf("  Logging out user: \033[1m%s\033[0m\n", current_username);
    printf("  ----------------------------------------------------------\n");
    
    // Clear login state
    user_logged_in = false;
    strcpy(current_username, "");
    
    printf("\n  [OK] Logout successful!\n");
    printf("  Thank you for using the system.\n");
    pause_execution();
}

/* ============================================================================
 * ID MANAGER SYSTEM
 * ============================================================================ */

/**
 * Helper function to get highest customer ID from file
 */
int get_highest_customer_id(void) {
    FILE *file;
    char line[300];
    char temp_id[10];
    int highest = 0;
    int current_id;
    
    file = fopen("data/customers.txt", "r");
    if (file == NULL) {
        return 0;  // No file exists, start from 0
    }
    
    // Read each line and find highest ID
    while (fgets(line, sizeof(line), file)) {
        // Get just the ID part (before first |)
        if (sscanf(line, "%[^|]", temp_id) == 1) {
            // Convert ID to number
            current_id = atoi(temp_id);
            if (current_id > highest) {
                highest = current_id;
            }
        }
    }
    
    fclose(file);
    return highest;
}

/**
 * Get next formatted ID
 */
int get_formatted_id(id_type_t type, char *buffer) {
    // Safety check
    if (buffer == NULL) {
        return 0;
    }
    
    // Simple if-else for each type
    if (type == ID_CUSTOMER) {
        // For customers, check existing file first time only
        if (customer_count == 0) {
            customer_count = get_highest_customer_id() + 1;
            if (customer_count == 1) {  // No existing customers
                customer_count = 1;
            }
        }
        sprintf(buffer, "%03d", customer_count);
        customer_count = customer_count + 1;
    }
    else if (type == ID_VEHICLE) {
        sprintf(buffer, "%03d", vehicle_count);
        vehicle_count = vehicle_count + 1;
    }
    else if (type == ID_SERVICE) {
        sprintf(buffer, "%03d", service_count_id);
        service_count_id = service_count_id + 1;
    }
    else if (type == ID_PARTS) {
        sprintf(buffer, "%03d", parts_count);
        parts_count = parts_count + 1;
    }
    else if (type == ID_INVOICE) {
        sprintf(buffer, "%03d", invoice_count_id);
        invoice_count_id = invoice_count_id + 1;
    }
    else {
        return 0;  // Wrong type
    }
    
    return 1;  // Success
}

// HRIDOY ENDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDddd

/* ============================================================================
 * CUSTOMER MANAGEMENT SYSTEM
 * ============================================================================ */

/**
 * Add a new customer with their vehicle
 */
int add_customer(void) {
    customer_t new_customer;
    FILE *file;
    
    printf("\n=== ADD NEW CUSTOMER & VEHICLE ===\n");
    printf("(Like real garage: Customer + Vehicle info together)\n\n");
    
    // Get automatic ID from ID manager
    if (!get_formatted_id(ID_CUSTOMER, new_customer.id)) {
        printf("Error: Could not generate customer ID\n");
        return 0;
    }
    
    printf("Customer ID: %s\n\n", new_customer.id);
    
    // === CUSTOMER INFORMATION ===
    printf("--- CUSTOMER INFORMATION ---\n");
    
    // Get customer name
    printf("Enter customer name: ");
    fgets(new_customer.name, sizeof(new_customer.name), stdin);
    new_customer.name[strcspn(new_customer.name, "\n")] = 0; // Remove newline
    
    // Get phone number
    printf("Enter phone number: ");
    fgets(new_customer.phone, sizeof(new_customer.phone), stdin);
    new_customer.phone[strcspn(new_customer.phone, "\n")] = 0; // Remove newline
    
    // Get address
    printf("Enter address: ");
    fgets(new_customer.address, sizeof(new_customer.address), stdin);
    new_customer.address[strcspn(new_customer.address, "\n")] = 0; // Remove newline
    
    // === VEHICLE INFORMATION ===
    printf("\n--- VEHICLE INFORMATION ---\n");
    
    // Get vehicle license plate
    printf("Enter vehicle license plate (e.g., ABC-123): ");
    fgets(new_customer.vehicle_plate, sizeof(new_customer.vehicle_plate), stdin);
    new_customer.vehicle_plate[strcspn(new_customer.vehicle_plate, "\n")] = 0; // Remove newline
    
    // Get vehicle make
    printf("Enter vehicle make/brand (e.g., Toyota): ");
    fgets(new_customer.vehicle_make, sizeof(new_customer.vehicle_make), stdin);
    new_customer.vehicle_make[strcspn(new_customer.vehicle_make, "\n")] = 0; // Remove newline
    
    // Get vehicle model
    printf("Enter vehicle model (e.g., Camry): ");
    fgets(new_customer.vehicle_model, sizeof(new_customer.vehicle_model), stdin);
    new_customer.vehicle_model[strcspn(new_customer.vehicle_model, "\n")] = 0; // Remove newline
    
    // Get vehicle year
    printf("Enter vehicle year (e.g., 2020): ");
    fgets(new_customer.vehicle_year, sizeof(new_customer.vehicle_year), stdin);
    new_customer.vehicle_year[strcspn(new_customer.vehicle_year, "\n")] = 0; // Remove newline
    
    // Get vehicle color
    printf("Enter vehicle color (e.g., Red): ");
    fgets(new_customer.vehicle_color, sizeof(new_customer.vehicle_color), stdin);
    new_customer.vehicle_color[strcspn(new_customer.vehicle_color, "\n")] = 0; // Remove newline
    
    // Save to file (simple append mode)
    file = fopen("data/customers.txt", "a");
    if (file == NULL) {
        printf("Error: Could not save customer data\n");
        return 0;
    }
    
    // Write customer + vehicle data in simple format: 
    // ID|Name|Phone|Address|Plate|Make|Model|Year|Color
    fprintf(file, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n", 
            new_customer.id, 
            new_customer.name, 
            new_customer.phone, 
            new_customer.address,
            new_customer.vehicle_plate,
            new_customer.vehicle_make,
            new_customer.vehicle_model,
            new_customer.vehicle_year,
            new_customer.vehicle_color);
    
    fclose(file);
    
    printf("\n[OK] Customer & Vehicle added successfully!\n");
    printf("------------------------------------------\n");
    printf("Customer ID: %s\n", new_customer.id);
    printf("Name: %s\n", new_customer.name);
    printf("Phone: %s\n", new_customer.phone);
    printf("Vehicle: %s %s %s (%s)\n", 
           new_customer.vehicle_year,
           new_customer.vehicle_make, 
           new_customer.vehicle_model,
           new_customer.vehicle_plate);
    printf("------------------------------------------\n");
    
    return 1;
}

/**
 * Display all customers with their vehicles
 */
void display_all_customers(void) {
    FILE *file;
    customer_t customer;
    char line[500];  // Larger buffer for more fields
    int count = 0;
    
    printf("\n=== ALL CUSTOMERS & THEIR VEHICLES ===\n");
    
    file = fopen("data/customers.txt", "r");
    if (file == NULL) {
        printf("No customers found or could not open file.\n");
        return;
    }
    
    printf("%-5s %-20s %-15s %-15s %-10s %-15s\n", 
           "ID", "Name", "Phone", "Plate", "Make", "Model");
    printf("--------------------------------------------------------------------------------\n");
    
    // Read each line from file
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (strlen(line) <= 1) continue;
        
        // Initialize fields to empty strings to handle missing data
        strcpy(customer.id, "");
        strcpy(customer.name, "");
        strcpy(customer.phone, "");
        strcpy(customer.address, "");
        strcpy(customer.vehicle_plate, "");
        strcpy(customer.vehicle_make, "");
        strcpy(customer.vehicle_model, "");
        strcpy(customer.vehicle_year, "");
        strcpy(customer.vehicle_color, "");
        
        // Parse line: ID|Name|Phone|Address|Plate|Make|Model|Year|Color
        int fields_parsed = sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]", 
                   customer.id, 
                   customer.name, 
                   customer.phone, 
                   customer.address,
                   customer.vehicle_plate,
                   customer.vehicle_make,
                   customer.vehicle_model,
                   customer.vehicle_year,
                   customer.vehicle_color);
        
        // Accept if we have at least the basic customer info (first 4 fields)
        if (fields_parsed >= 4 && strlen(customer.id) > 0) {
            
            printf("%-5s %-20s %-15s %-15s %-10s %-15s\n", 
                   customer.id, 
                   customer.name, 
                   customer.phone,
                   customer.vehicle_plate,
                   customer.vehicle_make,
                   customer.vehicle_model);
            count++;
        }
    }
    
    fclose(file);
    
    if (count == 0) {
        printf("No customers found.\n");
    } else {
        printf("--------------------------------------------------------------------------------\n");
        printf("Total customers: %d\n", count);
        printf("(Each customer registered with their vehicle)\n");
    }
}

/**
 * Search customer by ID
 */
int search_customer_by_id(char *search_id) {
    FILE *file;
    customer_t customer;
    char line[500];
    
    if (search_id == NULL) {
        return 0;
    }
    
    printf("\n=== SEARCH CUSTOMER BY ID ===\n");
    
    file = fopen("data/customers.txt", "r");
    if (file == NULL) {
        printf("No customers found or could not open file.\n");
        return 0;
    }
    
    // Read each line and compare ID
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (strlen(line) <= 1) continue;
        
        // Initialize fields to empty strings to handle missing data
        strcpy(customer.id, "");
        strcpy(customer.name, "");
        strcpy(customer.phone, "");
        strcpy(customer.address, "");
        strcpy(customer.vehicle_plate, "");
        strcpy(customer.vehicle_make, "");
        strcpy(customer.vehicle_model, "");
        strcpy(customer.vehicle_year, "");
        strcpy(customer.vehicle_color, "");
        
        // Parse line: ID|Name|Phone|Address|Plate|Make|Model|Year|Color
        int fields_parsed = sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]", 
                   customer.id, 
                   customer.name, 
                   customer.phone, 
                   customer.address,
                   customer.vehicle_plate,
                   customer.vehicle_make,
                   customer.vehicle_model,
                   customer.vehicle_year,
                   customer.vehicle_color);
        
        // Accept if we have at least the basic customer info and valid ID
        if (fields_parsed >= 4 && strlen(customer.id) > 0) {
            
            // Compare IDs (simple string comparison)
            if (strcmp(customer.id, search_id) == 0) {
                // Found the customer!
                printf("[OK] Customer Found!\n");
                printf("==========================================\n");
                printf("CUSTOMER INFORMATION:\n");
                printf("ID: %s\n", customer.id);
                printf("Name: %s\n", customer.name);
                printf("Phone: %s\n", customer.phone);
                printf("Address: %s\n", customer.address);
                printf("\nVEHICLE INFORMATION:\n");
                printf("License Plate: %s\n", customer.vehicle_plate);
                printf("Vehicle: %s %s %s\n", customer.vehicle_year, customer.vehicle_make, customer.vehicle_model);
                printf("Color: %s\n", customer.vehicle_color);
                printf("==========================================\n");
                
                fclose(file);
                return 1;
            }
        }
    }
    
    fclose(file);
    printf("[X] Customer with ID '%s' not found.\n", search_id);
    return 0;
}

/**
 * Customer & Vehicle management menu
 */
void customer_menu(void) {
    int choice;
    char search_id[10];
    
    while (1) {
        clear_screen();
        printf("\n");
        printf("================================================\n");
        printf("    CUSTOMER & VEHICLE MANAGEMENT MENU         \n");
        printf("================================================\n");
        printf("1. Add New Customer with Vehicle\n");
        printf("2. Display All Customers & Vehicles\n");
        printf("3. Search Customer by ID\n");
        printf("4. Back to Main Menu\n");
        printf("================================================\n");
        printf("Enter your choice (1-4): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        // Clear input buffer
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                add_customer();
                break;
                
            case 2:
                display_all_customers();
                break;
                
            case 3:
                printf("Enter customer ID to search: ");
                fgets(search_id, sizeof(search_id), stdin);
                search_id[strcspn(search_id, "\n")] = 0; // Remove newline
                search_customer_by_id(search_id);
                break;
                
            case 4:
                printf("Returning to main menu...\n");
                return;
                
            default:
                printf("Invalid choice! Please select 1-4.\n");
                break;
        }
        
        printf("\nPress Enter to continue...");
        getchar();
    }
}

/* ============================================================================
 * SERVICE MANAGEMENT SYSTEM
 * ============================================================================ */

/**
 * Function to get current date as string
 */
void getCurrentDate(char *date) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(date, 20, "%Y-%m-%d", tm);
}

/**
 * Simple function to check if customer exists
 */
int customerExists(int customer_id) {
    char customer_id_str[10];
    snprintf(customer_id_str, sizeof(customer_id_str), "%03d", customer_id);  // Convert to "001" format
    return search_customer_by_id(customer_id_str);  // Returns 1 if found, 0 if not
}

/**
 * Add a new service
 */
void addService(void) {
    if (service_count >= MAX_SERVICES) {
        printf("\nError: Maximum service limit reached!\n");
        return;
    }

    Service new_service;
    
    // Get customer ID
    printf("\nEnter Customer ID: ");
    scanf("%d", &new_service.customer_id);
    getchar(); // Clear input buffer

    // Check if customer exists - simple validation
    if (!customerExists(new_service.customer_id)) {
        printf("Warning: Customer ID %d not found in system!\n", new_service.customer_id);
        printf("Do you want to continue anyway? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        getchar(); // Clear input buffer
        
        if (choice != 'y' && choice != 'Y') {
            printf("Service creation cancelled.\n");
            pause_execution();
            return;
        }
    }

    // Get service details
    printf("Enter Service Name: ");
    scanf(" %[^\n]", new_service.service_name);
    
    printf("Enter Service Cost: $");
    scanf("%lf", &new_service.cost);

    // Set service ID in 001, 002, 003 format - simple and easy
    // Ensure we don't exceed 999 services (3-digit limit)
    if (service_count + 1 > 999) {
        printf("Error: Service ID limit reached (999 services maximum)!\n");
        pause_execution();
        return;
    }
    snprintf(new_service.service_id, sizeof(new_service.service_id), "%03d", service_count + 1);
    getCurrentDate(new_service.date);  // Set current date
    strcpy(new_service.status, "Pending");  // Initial status

    // Add to array
    services[service_count] = new_service;
    service_count++;

    // Save to file (simple text format)
    FILE *file = fopen("data/services.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < service_count; i++) {
            fprintf(file, "%s|%d|%s|%s|%s|%.2f\n",
                    services[i].service_id,
                    services[i].customer_id,
                    services[i].service_name,
                    services[i].date,
                    services[i].status,
                    services[i].cost);
        }
        fclose(file);
    }

    printf("\n[OK] Service added successfully!\n");
    printf("Service ID: %s\n", new_service.service_id);
    pause_execution();
}

/**
 * View all services
 */
void viewAllServices(void) {
    if (service_count == 0) {
        printf("\nNo services found.\n");
        pause_execution();
        return;
    }

    printf("\n=== All Services ===\n");
    printf("ID  | Customer ID | Service Name        | Date       | Status     | Cost\n");
    printf("----+-------------+-------------------+------------+-----------+--------\n");

    for (int i = 0; i < service_count; i++) {
        printf("%-3s | %-11d | %-17s | %-10s | %-9s | $%.2f\n",
            services[i].service_id,
            services[i].customer_id,
            services[i].service_name,
            services[i].date,
            services[i].status,
            services[i].cost);
    }
    printf("\n");
    pause_execution();
}

/**
 * Update service status
 */
void updateServiceStatus(void) {
    char service_id[10];
    printf("\nEnter Service ID to update (like 001): ");
    scanf("%s", service_id);

    // Find the service
    int found = 0;
    for (int i = 0; i < service_count; i++) {
        if (strcmp(services[i].service_id, service_id) == 0) {
            printf("\nCurrent Status: %s\n", services[i].status);
            printf("\nSelect New Status:\n");
            printf("1. Pending\n");
            printf("2. In Progress\n");
            printf("3. Completed\n");
            
            int choice;
            printf("Enter choice (1-3): ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    strcpy(services[i].status, "Pending");
                    break;
                case 2:
                    strcpy(services[i].status, "In Progress");
                    break;
                case 3:
                    strcpy(services[i].status, "Completed");
                    break;
                default:
                    printf("Invalid choice!\n");
                    pause_execution();
                    return;
            }

            // Save changes to file (simple text format)
            FILE *file = fopen("data/services.txt", "w");
            if (file != NULL) {
                for (int j = 0; j < service_count; j++) {
                    fprintf(file, "%s|%d|%s|%s|%s|%.2f\n",
                            services[j].service_id,
                            services[j].customer_id,
                            services[j].service_name,
                            services[j].date,
                            services[j].status,
                            services[j].cost);
                }
                fclose(file);
            }

            printf("\n[OK] Service status updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("\nService ID '%s' not found!\n", service_id);
    }
    pause_execution();
}

/**
 * Load services from simple text file
 */
static void loadServices(void) {
    FILE *file = fopen("data/services.txt", "r");
    if (file == NULL) {
        service_count = 0;
        return;
    }
    
    service_count = 0;
    char line[300];
    
    while (fgets(line, sizeof(line), file) && service_count < MAX_SERVICES) {
        // Parse format: ServiceID|CustomerID|ServiceName|Date|Status|Cost
        if (sscanf(line, "%19[^|]|%d|%99[^|]|%19[^|]|%19[^|]|%lf", 
                   services[service_count].service_id,
                   &services[service_count].customer_id,
                   services[service_count].service_name,
                   services[service_count].date,
                   services[service_count].status,
                   &services[service_count].cost) >= 6) {
            service_count++;
        }
    }
    
    fclose(file);
}

/**
 * Service Management Menu
 */
void serviceManagementMenu(void) {
    // Load services when entering the menu
    loadServices();

    int choice;
    do {
        clear_screen();
        printf("\n=== Service Management ===\n");
        printf("1. Add New Service\n");
        printf("2. View All Services\n");
        printf("3. Update Service Status\n");
        printf("0. Back to Main Menu\n");
        printf("\nEnter your choice: ");
        
        scanf("%d", &choice);
        getchar(); // Clear input buffer

        switch (choice) {
            case 1:
                addService();
                break;
            case 2:
                viewAllServices();
                break;
            case 3:
                updateServiceStatus();
                break;
            case 0:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                pause_execution();
        }
    } while (choice != 0);
}


// FINISH  SHAFIMmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
/* ============================================================================
 * BILLING MANAGEMENT SYSTEM
 * ============================================================================ */

/**
 * Get today's date - simple function
 */
void getCurrentDateSimple(char *date) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(date, 20, "%Y-%m-%d", tm);
}

/**
 * Load invoices from simple text file
 */
void loadSimpleInvoices(void) {
    FILE *file = fopen("data/invoices.txt", "r");
    if (file == NULL) {
        invoice_count = 0;
        return;
    }

    invoice_count = 0;
    char line[400];
    
    while (fgets(line, sizeof(line), file) && invoice_count < MAX_INVOICES) {
        // Simple format: INV001|SRV001|123|John Doe|Engine Service|2025-08-07|150.00|Pending
        char *token = strtok(line, "|");
        if (token) strcpy(invoices[invoice_count].invoice_id, token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(invoices[invoice_count].service_id, token);
        
        token = strtok(NULL, "|");
        if (token) invoices[invoice_count].customer_id = atoi(token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(invoices[invoice_count].customer_name, token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(invoices[invoice_count].service_name, token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(invoices[invoice_count].invoice_date, token);
        
        token = strtok(NULL, "|");
        if (token) invoices[invoice_count].total_amount = atof(token);
        
        token = strtok(NULL, "|\n");
        if (token) strcpy(invoices[invoice_count].payment_status, token);
        
        invoice_count++;
    }
    
    fclose(file);
}

/**
 * Save invoices to simple text file
 */
void saveSimpleInvoices(void) {
    FILE *file = fopen("data/invoices.txt", "w");
    if (file == NULL) {
        printf("Error: Could not save invoices!\n");
        return;
    }

    for (int i = 0; i < invoice_count; i++) {
        fprintf(file, "%s|%s|%d|%s|%s|%s|%.2f|%s\n",
                invoices[i].invoice_id,
                invoices[i].service_id,
                invoices[i].customer_id,
                invoices[i].customer_name,
                invoices[i].service_name,
                invoices[i].invoice_date,
                invoices[i].total_amount,
                invoices[i].payment_status);
    }
    
    fclose(file);
}

/**
 * Simple invoice generation - main feature
 */
void generateSimpleInvoice(void) {
    printf("\n=== Generate Simple Invoice ===\n");
    
    // Step 1: Read completed services from text file
    FILE *service_file = fopen("data/services.txt", "r");
    if (service_file == NULL) {
        printf("No services found! Please add services first.\n");
        pause_execution();
        return;
    }

    // Load services from text file
    Service services_temp[50];
    int service_count_temp = 0;
    char line[300];
    
    while (fgets(line, sizeof(line), service_file) && service_count_temp < 50) {
        // Parse format: ServiceID|CustomerID|ServiceName|Date|Status|Cost
        if (sscanf(line, "%19[^|]|%d|%99[^|]|%19[^|]|%19[^|]|%lf", 
                   services_temp[service_count_temp].service_id,
                   &services_temp[service_count_temp].customer_id,
                   services_temp[service_count_temp].service_name,
                   services_temp[service_count_temp].date,
                   services_temp[service_count_temp].status,
                   &services_temp[service_count_temp].cost) >= 6) {
            service_count_temp++;
        }
    }
    
    fclose(service_file);

    // Step 2: Show completed services
    printf("\n--- Completed Services ---\n");
    printf("No. | Service ID | Customer ID | Service Name    | Cost\n");
    printf("----+------------+-------------+----------------+--------\n");

    int completed_services[50];
    int completed_count = 0;

    for (int i = 0; i < service_count_temp; i++) {
        if (strcmp(services_temp[i].status, "Completed") == 0) {
            printf("%-3d | %-10s | %-11d | %-14s | $%.2f\n",
                   completed_count + 1,
                   services_temp[i].service_id,
                   services_temp[i].customer_id,
                   services_temp[i].service_name,
                   services_temp[i].cost);
            completed_services[completed_count] = i;
            completed_count++;
        }
    }

    if (completed_count == 0) {
        printf("No completed services found!\n");
        pause_execution();
        return;
    }

    // Step 3: User picks a service
    printf("\nSelect service number (1-%d): ", completed_count);
    int choice;
    scanf("%d", &choice);
    getchar();

    if (choice < 1 || choice > completed_count) {
        printf("Invalid choice!\n");
        pause_execution();
        return;
    }

    Service selected_service = services_temp[completed_services[choice - 1]];

    // Step 4: Get customer name
    char customer_name[100] = "Unknown Customer";
    FILE *customer_file = fopen("data/customers.txt", "r");
    if (customer_file != NULL) {
        char line[500];
        while (fgets(line, sizeof(line), customer_file)) {
            int file_customer_id;
            char name[100];
            // Simple parsing - just get ID and name
            if (sscanf(line, "%d|%99[^|]", &file_customer_id, name) >= 2) {
                if (file_customer_id == selected_service.customer_id) {
                    strcpy(customer_name, name);
                    break;
                }
            }
        }
        fclose(customer_file);
    }

    // Step 5: Check if service has cost
    if (selected_service.cost <= 0) {
        printf("\nService has no cost! Cannot create invoice.\n");
        pause_execution();
        return;
    }

    // Step 6: Create simple invoice
    if (invoice_count >= MAX_INVOICES) {
        printf("Invoice limit reached!\n");
        pause_execution();
        return;
    }

    Invoice new_invoice;
    sprintf(new_invoice.invoice_id, "INV%03d", invoice_count + 1);
    strcpy(new_invoice.service_id, selected_service.service_id);
    new_invoice.customer_id = selected_service.customer_id;
    strcpy(new_invoice.customer_name, customer_name);
    strcpy(new_invoice.service_name, selected_service.service_name);
    getCurrentDateSimple(new_invoice.invoice_date);
    new_invoice.total_amount = selected_service.cost;  // No tax - keep it simple
    strcpy(new_invoice.payment_status, "Paid");

    // Step 7: Save invoice
    invoices[invoice_count] = new_invoice;
    invoice_count++;
    saveSimpleInvoices();

    // Step 8: Show success and ask to print
    printf("\n[OK] Invoice created successfully!\n");
    printf("Invoice ID: %s\n", new_invoice.invoice_id);
    printf("Total Amount: $%.2f\n", new_invoice.total_amount);
    
    printf("\nPrint receipt? (y/n): ");
    char print_choice;
    scanf(" %c", &print_choice);
    getchar();
    
    if (print_choice == 'y' || print_choice == 'Y') {
        // Print simple receipt inline
        printf("\n");
        printf("========================================\n");
        printf("          GARAGE INVOICE RECEIPT        \n");
        printf("========================================\n");
        printf("\n");
        printf("Invoice ID: %s\n", new_invoice.invoice_id);
        printf("Date: %s\n", new_invoice.invoice_date);
        printf("\n");
        printf("Customer: %s (ID: %d)\n", new_invoice.customer_name, new_invoice.customer_id);
        printf("Service: %s\n", new_invoice.service_name);
        printf("Service ID: %s\n", new_invoice.service_id);
        printf("\n");
        printf("Total Amount: $%.2f\n", new_invoice.total_amount);
        printf("Payment Status: %s\n", new_invoice.payment_status);
        printf("\n");
        printf("     Thank you for your business!\n");
        printf("========================================\n");
        printf("\n");
    }
    
    pause_execution();
}

/**
 * View all invoices - simple table
 */
void viewAllInvoices(void) {
    printf("\n=== All Invoices ===\n");
    
    if (invoice_count == 0) {
        printf("No invoices found.\n");
        pause_execution();
        return;
    }

    printf("Invoice ID | Customer Name       | Service Name    | Amount  | Status\n");
    printf("-----------+--------------------+----------------+---------+--------\n");

    for (int i = 0; i < invoice_count; i++) {
        printf("%-10s | %-18s | %-14s | $%6.2f | %s\n",
               invoices[i].invoice_id,
               invoices[i].customer_name,
               invoices[i].service_name,
               invoices[i].total_amount,
               invoices[i].payment_status);
    }
    
    pause_execution();
}

/**
 * Simple billing menu
 */
void billingManagementMenu(void) {
    loadSimpleInvoices();

    int choice;
    do {
        clear_screen();
        printf("\n=== Simple Billing System ===\n");
        printf("1. Generate Invoice\n");
        printf("2. View All Invoices\n");
        printf("0. Back to Main Menu\n");
        printf("\nEnter your choice: ");
        
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                generateSimpleInvoice();
                break;
            case 2:
                viewAllInvoices();
                break;
            case 0:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice!\n");
                pause_execution();
        }
    } while (choice != 0);
}

/* ============================================================================
 * REPORTS SYSTEM
 * ============================================================================ */

/**
 * Get today's date for report
 */
void getCurrentReportDate(char *date) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(date, 20, "%Y-%m-%d", tm);
}

/**
 * Daily Revenue Report - main feature
 */
void dailyRevenueReport(void) {
    printf("\n=== Daily Revenue Report ===\n");
    
    char today[20];
    getCurrentReportDate(today);
    printf("Date: %s\n", today);
    printf("--------------------------------------\n");
    
    // Read invoices from file
    FILE *file = fopen("data/invoices.txt", "r");
    if (file == NULL) {
        printf("No invoice data found.\n");
        printf("Total Revenue Today: $0.00\n");
        pause_execution();
        return;
    }
    
    char line[500];
    double total_revenue = 0.0;
    int invoice_count_today = 0;
    
    printf("\nInvoice Details for Today:\n");
    printf("Invoice ID | Customer Name       | Service Name    | Amount  | Status\n");
    printf("-----------+--------------------+----------------+---------+--------\n");
    
    while (fgets(line, sizeof(line), file)) {
        char invoice_id[20], service_id[20], customer_name[100];
        char service_name[100], invoice_date[20], payment_status[20];
        int customer_id;
        double amount;
        
        // Parse invoice line: INV001|SRV001|123|John Doe|Engine Service|2025-08-07|150.00|Pending
        if (sscanf(line, "%19[^|]|%19[^|]|%d|%99[^|]|%99[^|]|%19[^|]|%lf|%19[^|]", 
                   invoice_id, service_id, &customer_id, customer_name, 
                   service_name, invoice_date, &amount, payment_status) >= 7) {
            
            // Check if invoice is from today
            if (strcmp(invoice_date, today) == 0) {
                printf("%-10s | %-18s | %-14s | $%6.2f | %s\n",
                       invoice_id, customer_name, service_name, amount, payment_status);
                total_revenue += amount;
                invoice_count_today++;
            }
        }
    }
    
    fclose(file);
    
    if (invoice_count_today == 0) {
        printf("No invoices generated today.\n");
    }
    
    printf("\n--------------------------------------\n");
    printf("Total Invoices Today: %d\n", invoice_count_today);
    printf("Total Revenue Today: $%.2f\n", total_revenue);
    printf("--------------------------------------\n");
    
    pause_execution();
}

/**
 * Simple reports menu
 */
void reportsMenu(void) {
    int choice;
    do {
        clear_screen();
        printf("\n=== Reports Menu ===\n");
        printf("1. Daily Revenue Report\n");
        printf("0. Back to Main Menu\n");
        printf("\nEnter your choice: ");
        
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                dailyRevenueReport();
                break;
            case 0:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                pause_execution();
        }
    } while (choice != 0);
}

/* ============================================================================
 * MAIN PROGRAM
 * ============================================================================ */

/**
 * Display the main menu options
 */
void display_main_menu(void) {
    clear_screen();
    
    if (!is_user_logged_in()) {
        // Show enhanced welcome header with integrated design
        printf("\n");
        printf("  +==================================================================+\n");
        printf("  |                                                                  |\n");
        printf("  |             \033[1mGARAGE SERVICE MANAGEMENT SYSTEM\033[0m                |\n");
        printf("  |                     Professional Edition                        |\n");
        printf("  |                                                                  |\n");
        printf("  |    \033[1m\033[36m   ___  ___  __  __ ___      \033[1m\033[33mVERSION 1.0\033[0m    |\n");
        printf("  |    \033[1m\033[36m  / __|| __||  \\/  / __|     \033[0m                    |\n");
        printf("  |    \033[1m\033[36m | (_ ||__ \\| |\\/| \\__ \\     \033[1m\033[32m* ONLINE\033[0m      |\n");
        printf("  |    \033[1m\033[36m  \\___||___/|_|  |_|___/     \033[0m                    |\n");
        printf("  |    \033[1m\033[36m                             \033[1m\033[37m2025 EDITION\033[0m|\n");
        printf("  |    \033[1m\033[36m                              \033[0m                    |\n");
        printf("  |                                                                  |\n");
        printf("  +==================================================================+\n");
        printf("  +------------------------------------------------------------------+\n");
        printf("  |                    \033[1m\033[31m[*] AUTHENTICATION REQUIRED\033[0m                |\n");
        printf("  +------------------------------------------------------------------+\n");
        printf("  |                                                                  |\n");
        printf("  |  Welcome to GSMS! Please authenticate to access the system      |\n");
        printf("  |                                                                  |\n");
        printf("  |                                                                  |\n");
        printf("  |  \033[1mAvailable Options:\033[0m                                          |\n");
        printf("  |    \033[1m[\033[0m\033[32m1\033[0m\033[1m]\033[0m Login to System                                    |\n");
        printf("  |    \033[1m[\033[0m\033[31m0\033[0m\033[1m]\033[0m Exit Application                                   |\n");
        printf("  |                                                                  |\n");
        printf("  +------------------------------------------------------------------+\n");
    } else {
        // Show enhanced main application menu
        printf("\n");
        printf("    +==========================================================+\n");
        printf("    |                                                          |\n");
        printf("    |    \033[1m\033[36m   ___  ___  __  __ ___   \033[0m \033[1m\033[32m* SYSTEM ACTIVE\033[0m|\n");
        printf("    |    \033[1m\033[36m  / __|| __||  \\/  / __|  \033[0m                 |\n");
        printf("    |    \033[1m\033[36m | (_ ||__ \\| |\\/| \\__ \\  \033[0m \033[1mMANAGEMENT\033[0m     |\n");
        printf("    |    \033[1m\033[36m  \\___||___/|_|  |_|___/  \033[0m \033[1mSYSTEM\033[0m        |\n");
        printf("    |    \033[1m\033[36m                          \033[0m                 |\n");
        printf("    |    \033[1m\033[36m                             \033[0m                 |\n");
        printf("    |                                                          |\n");
        printf("    +==========================================================+\n");
        printf("\n");
        printf("    \033[1m>> Welcome back, \033[1m\033[33m%s\033[0m\n", current_username);
        printf("    ----------------------------------------------------------\n");
        printf("\n");
        printf("    \033[1m[*] SYSTEM MODULES:\033[0m\n");
        printf("\n");
        printf("      \033[1m[\033[0m\033[36m1\033[0m\033[1m]\033[0m  >> Customer & Vehicle Management\n");
        printf("      \033[1m[\033[0m\033[36m2\033[0m\033[1m]\033[0m  >> Service Management\n");
        printf("      \033[1m[\033[0m\033[36m3\033[0m\033[1m]\033[0m  >> Billing & Invoices\n");
        printf("      \033[1m[\033[0m\033[36m4\033[0m\033[1m]\033[0m  >> Reports\n");
        printf("\n");
        printf("    \033[1m[*] SYSTEM OPTIONS:\033[0m\n");
        printf("\n");
        printf("      \033[1m[\033[0m\033[31m5\033[0m\033[1m]\033[0m  >> Logout\n");
        printf("      \033[1m[\033[0m\033[31m0\033[0m\033[1m]\033[0m  >> Exit\n");
        printf("\n");
    }
}

/**
 * Get user's menu choice with validation
 */
int get_menu_choice(void) {
    int choice;
    printf("Enter your choice: ");
    
    if (scanf("%d", &choice) != 1) {
        // Clear invalid input
        clear_input_buffer();
        return -1;  // Invalid input
    }
    
    clear_input_buffer();  // Clear any remaining input
    return choice;
}

/**
 * Handle the user's menu choice
 */
void handle_menu_choice(int choice) {
    if (!is_user_logged_in()) {
        // User not logged in
        switch (choice) {
            case 1:
                if (login_user()) {
                    printf("[OK] Login successful! Redirecting to main menu...\n");                   
                } else {
                    printf("[X] Login failed. Please try again.\n");
                    pause_execution();
                }
                break;
            case 0:
                printf("\n>> Goodbye! Thank you for using GSMS.\n");
                break;
            default:
                printf("\n[X] Invalid choice! Please select 1 to login or 0 to exit.\n");
                pause_execution();
                break;
        }
    } else {
        // User logged in
        switch (choice) {
            case 1:
                printf("\n[*] CUSTOMER AND VEHICLES MANAGEMENT\n");
                printf("----------------------------------------\n");
                customer_menu();
                break;
            case 2:
                printf("\n[*] SERVICE MANAGEMENT\n");
                printf("--------------------------\n");
                serviceManagementMenu();
                break;
            case 3:
                printf("\n[*] BILLING & INVOICES\n");
                printf("--------------------------\n");
                billingManagementMenu();
                break;
            case 4:
                printf("\n[*] REPORTS\n");
                printf("--------------------------\n");
                reportsMenu();
                break;
            case 5:
                logout_user();
                break;
            case 0:
                printf("\n>> Goodbye! Thank you for using GSMS.\n");
                break;
            default:
                printf("\n[X] Invalid choice! Please select a number from 0-5.\n");
                pause_execution();
                break;
        }
    }
}

/**
 * Initialize system components
 */
void initialize_system(void) {
    // Create data directory if it doesn't exist (silently)
    create_data_directory();
    
    // Initialize simple authentication
    user_logged_in = false;
    strcpy(current_username, "");
}

/**
 * Cleanup system resources before exit
 */
void cleanup_system(void) {
    printf("\n[*] Cleaning up system resources...\n");
    
    // Logout user if still logged in
    if (is_user_logged_in()) {
        printf("[*] Logging out current user...\n");
        user_logged_in = false;
        strcpy(current_username, "");
    }
    
    printf("[OK] System cleanup completed.\n");
}

/**
 * Main function - Program entry point
 */
int main(void) {
    // Initialize system components (silently)
    initialize_system();
    
    // Main program loop
    int choice;
    do {
        display_main_menu();
        choice = get_menu_choice();
        handle_menu_choice(choice);
    } while (choice != 0);
    
    // Cleanup before exit
    cleanup_system();
    
    printf("\nThank you for using GSMS!\n");
    printf("Goodbye!\n");
    
    return 0;
}

/* ============================================================================
 * END OF GARAGE MANAGEMENT SYSTEM
 * ============================================================================ */