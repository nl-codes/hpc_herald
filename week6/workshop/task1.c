#include <stdio.h>
#include <stdlib.h>

// Define total number of products available
#define TOTAL_PRODUCTS 10
// Define maximum number of items that can be added to cart
#define TOTAL_CART 20

// Global variable to track current number of items in cart
int cartCount = 0;

// Product structure definition to store product information
typedef struct {
    int productId;           // Unique product identifier
    char name[30];           // Product name
    float price;             // Product price
    int taxable;             // Flag: 1 if taxable, 0 if not
} Product;

// Configuration structure for tax settings
typedef struct {
    float tax_rate;          // Tax rate percentage
} setting_conf;

// Cart structure to store items added to cart
typedef struct {
    int productId;           // Product ID in cart
    int quantity;            // Quantity of product ordered
} Cart;


// Function prototype: Initialize product list with predefined products
void createProducts(Product* product_list);
// Function prototype: Display all available products to the user
void displayProductList(Product* product_list);
// Function prototype: Get product orders from user input
void getProductOrders(Cart* cart_list, Product* product_list);
// Function prototype: Calculate total price of all items in cart
float getGrandTotal(Cart* cart_list, Product* product_list);
// Function prototype: Print invoice with cart details
void printInvoice(int invoiceNum, float totalPrice, Cart* cart_list, Product* product_list);

// Main function - entry point of the program
int main() {
    // Array to store all available products
    Product products[TOTAL_PRODUCTS];
    // Array to store items added to cart
    Cart cartItems[TOTAL_CART];

    // Tax not required for task 1
    // // Initialize tax configuration with 13% tax rate
    // setting_conf tax = { 0.13 };
    // Variable to store invoice number
    int invoiceNumber;
    // Variable to store grand total of cart
    float grandTotal;

    // Prompt user to enter invoice number
    printf("Enter invoice number: ");
    scanf("%d", &invoiceNumber);

    // Initialize product list with predefined products
    createProducts(products);
    // Get product orders from user
    getProductOrders(cartItems, products);
    // Calculate grand total of all items in cart
    grandTotal = getGrandTotal(cartItems, products);
    // Print invoice with cart details
    printInvoice(invoiceNumber, grandTotal, cartItems, products);

    return 0;
}

// Function to create and initialize product list with 10 products
void createProducts(Product* product_list) {
    // Temporary array with predefined product data
    Product temp[TOTAL_PRODUCTS] = {
        {.taxable = 1, .price = 49.99, .productId = 1,  .name = "Zanpakuto Replica"},
        {.taxable = 1, .price = 89.50, .productId = 2,  .name = "Shihakusho Outfit"},
        {.taxable = 1, .price = 12.75, .productId = 3,  .name = "Soul Candy Shower"},
        {.taxable = 1, .price = 99.99, .productId = 4,  .name = "Hogyoku Crystal"},
        {.taxable = 1, .price = 25.00, .productId = 5,  .name = "Substitute Badge"},
        {.taxable = 1, .price = 18.99, .productId = 6,  .name = "Quincy Cross"},
        {.taxable = 1, .price = 59.99, .productId = 7,  .name = "Hollows Mask Prop"},
        {.taxable = 1, .price = 45.49, .productId = 8,  .name = "Reiatsu Reader"},
        {.taxable = 1, .price = 22.00, .productId = 9,  .name = "Urahara Hat"},
        {.taxable = 0, .price = 15.99, .productId = 10,  .name = "Kon Plush Toy"}
    };

    // Copy products from temp array to the product_list parameter
    for (int i = 0; i < TOTAL_PRODUCTS; i++) {
        product_list[i] = temp[i];
    }
}

// Function to display all available products in a formatted table
void displayProductList(Product* product_list) {
    printf("\n================= PRODUCT LIST 👇=================\n");
    // Print table header
    printf("ID   Name                            Price   Taxable\n");
    // Loop through all products and print their details
    for (int i = 0; i < TOTAL_PRODUCTS; i++) {
        printf("%-4d %-30s %-7.2f    %d\n", product_list[i].productId, product_list[i].name, product_list[i].price, product_list[i].taxable);
    }
    printf("\n================= PRODUCT LIST ☝️ =================\n");
}

// Function to get product orders from user input
void getProductOrders(Cart* cart_list, Product* product_list) {
    // Display available products to user
    displayProductList(product_list);
    // Loop to allow user to add multiple items
    while (1) {
        int pid, pquant;

        // Prompt user to enter product ID
        printf("\nEnter product ID: ");
        scanf("%d", &pid);

        // Validate if product ID is within valid range
        if (pid > TOTAL_PRODUCTS) {
            printf("Invalid Product id!\n");
            continue;
        }
        // Prompt user to enter quantity
        printf("Enter quantity: ");
        scanf("%d", &pquant);

        // Add item to cart
        cart_list[cartCount].productId = pid;
        cart_list[cartCount].quantity = pquant;
        cartCount++;

        // Ask user if they want to add another item
        char choice;
        printf("Add another item (y/n): ");
        scanf(" %c", &choice);

        // Exit loop if user enters 'n' or 'N'
        if (choice == 'n' || choice == 'N')
            break;
        // Display product list again for next selection
        displayProductList(product_list);
    }
}

// Function to calculate grand total of all items in cart
float getGrandTotal(Cart* cart_list, Product* product_list) {
    // Initialize total to zero
    float total = 0;

    // Loop through all items in cart
    for (int i = 0; i < cartCount; i++) {
        // Get product ID from current cart item
        int pid = cart_list[i].productId;

        // Search for matching product in product list
        for (int j = 0; j < TOTAL_PRODUCTS; j++) {
            // If product found, add to total (quantity * price)
            if (product_list[j].productId == pid) {
                total += cart_list[i].quantity * product_list[j].price;
                break;
            }
        }
    }
    // Return calculated total
    return total;
}


// Function to print invoice with all cart details
void printInvoice(int invoiceNum, float totalPrice, Cart* cart_list, Product* product_list) {
    // Print invoice header
    printf("\n==================== INVOICE ====================\n");
    printf("Invoice No: %d\n", invoiceNum);
    printf("-------------------------------------------------\n");
    // Print table header for invoice items
    printf("SN  Product Name        Price   Qty   Total\n");
    printf("-------------------------------------------------\n");

    // Loop through all items in cart to print details
    for (int i = 0; i < cartCount; i++) {
        // Get product ID and quantity from cart
        int pid = cart_list[i].productId;
        int qty = cart_list[i].quantity;

        // Find matching product in product list
        Product p;
        for (int j = 0; j < TOTAL_PRODUCTS; j++) {
            // If product found, store it in variable p
            if (product_list[j].productId == pid) {
                p = product_list[j];
                break;
            }
        }

        // Calculate total price for this item (price * quantity)
        float total = p.price * qty;
        // Print invoice line for current item
        printf("%-3d %-18s %-7.2f %-5d %.2f\n",
            i + 1, p.name, p.price, qty, total);
    }

    // Print invoice footer with grand total
    printf("-------------------------------------------------\n");
    printf("Grand Total: %.2f\n", totalPrice);
    printf("=================================================\n");
}
