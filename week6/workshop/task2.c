#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

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
    float totalWithoutTax;   // Total without tax (price * quantity)
    float tax;               // Tax : setting_config.tax_rate % of totalWithoutTax
    float totalWithTax;      // Final Total tax + totalWithoutTax
} Cart;

// Function prototype: Initialize product list with predefined products
void createProducts(Product* product_list);
// Function prototype: Display all available products to the user
void displayProductList(Product* product_list);
// Function prototype: Get product orders from user input
void getProductOrders(Cart* cart_list, Product* product_list);
// Function prototype: Calculate tax for each cart item based on taxable flag using OpenMP
void calculateTax(Cart* cart_list, Product* product_list, setting_conf tax);
// Function prototype: Calculate subtotal and total tax from cart items using OpenMP
void getSubTotalAndTax(Cart* cart_list, float* totalTax, float* subTotal);
// Function prototype: Print invoice with cart details and tax calculation
void printInvoice(int invoiceNum, Cart* cart_list, Product* product_list);

// Main function - entry point of the program
int main() {
    // Array to store all available products
    Product products[TOTAL_PRODUCTS];
    // Array to store items added to cart
    Cart cartItems[TOTAL_CART];

    // Initialize tax configuration with 13% tax rate
    setting_conf tax = { 0.13 };
    // Variable to store invoice number
    int invoiceNumber;

    // Prompt user to enter invoice number
    printf("Enter invoice number: ");
    scanf("%d", &invoiceNumber);

    // Initialize product list with predefined products
    createProducts(products);
    // Get product orders from user
    getProductOrders(cartItems, products);
    // Calculate tax for each cart item using parallel processing
    calculateTax(cartItems, products, tax);
    // Print invoice with cart details and tax calculation
    printInvoice(invoiceNumber, cartItems, products);

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
    printf("\n================= PRODUCT LIST 👇====================\n");
    // Print table header
    printf("ID   Name                            Price   Taxable\n");
    // Loop through all products and print their details
    for (int i = 0; i < TOTAL_PRODUCTS; i++) {
        printf("%-4d %-30s %-7.2f    %d\n", product_list[i].productId, product_list[i].name, product_list[i].price, product_list[i].taxable);
    }
    printf("\n================= PRODUCT LIST ☝️ ====================\n");
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

        // Add item to cart with default values for tax fields
        cart_list[cartCount].productId = pid;
        cart_list[cartCount].quantity = pquant;
        cart_list[cartCount].totalWithoutTax = 0;
        cart_list[cartCount].tax = 0;
        cart_list[cartCount].totalWithTax = 0;
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

// Function to calculate tax for each cart item using parallel processing with OpenMP
void calculateTax(Cart* cart_list, Product* product_list, setting_conf tax) {
    // Parallel loop to calculate tax for all cart items using OpenMP
    // Each thread processes a different cart item independently
#pragma omp parallel for collapse(1) shared(cart_list, product_list, tax)
    for (int i = 0; i < cartCount; i++) {
        // Get product ID from current cart item
        int pid = cart_list[i].productId;
        // Get quantity from current cart item
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

        // Calculate total without tax (price * quantity)
        cart_list[i].totalWithoutTax = p.price * qty;

        // Calculate tax amount only if product is taxable
        cart_list[i].tax = 0;
        if (p.taxable == 1) {
            // Calculate tax as percentage of total without tax
            cart_list[i].tax = cart_list[i].totalWithoutTax * tax.tax_rate;
        }

        // Calculate total with tax (totalWithoutTax + tax)
        cart_list[i].totalWithTax = cart_list[i].totalWithoutTax + cart_list[i].tax;
    }
}

// Function to calculate subtotal and total tax from all cart items using OpenMP
void getSubTotalAndTax(Cart* cart_list, float* totalTax, float* subTotal) {
    // Initialize local variables for parallel reduction
    float local_subTotal = 0;
    float local_totalTax = 0;

    // Parallel loop to accumulate subtotal and total tax using OpenMP
    // Uses reduction clause to safely combine partial results from all threads
#pragma omp parallel for collapse(1) reduction(+:local_subTotal, local_totalTax)
    for (int i = 0; i < cartCount; i++) {
        // Add totalWithoutTax to subtotal using reduction
        local_subTotal += cart_list[i].totalWithoutTax;
        // Add tax to totalTax using reduction
        local_totalTax += cart_list[i].tax;
    }

    // Assign calculated local values back to pointer parameters
    *subTotal = local_subTotal;
    *totalTax = local_totalTax;
}

// Function to print invoice with all cart details including tax calculation
void printInvoice(int invoiceNum, Cart* cart_list, Product* product_list) {
    // Print invoice header
    printf("\n==================== INVOICE ======================================\n");
    printf("Invoice No: %d\n", invoiceNum);
    printf("-------------------------------------------------\n");
    // Print table header for invoice items with tax column
    printf("SN  Product Name        Price   Qty   Total   Tax Amount Final Total\n");
    printf("-------------------------------------------------\n");

    // Initialize variables to track subtotal and total tax
    float subTotal = 0;
    float totalTax = 0;

    // Sequential loop to print invoice items (output must be sequential)
    for (int i = 0; i < cartCount; i++) {
        // Get product ID from current cart item
        int pid = cart_list[i].productId;

        // Find matching product in product list
        Product p;
        for (int j = 0; j < TOTAL_PRODUCTS; j++) {
            // If product found, store it in variable p
            if (product_list[j].productId == pid) {
                p = product_list[j];
                break;
            }
        }

        // Print invoice line for current item with tax amount and final total
        printf("%-3d %-18s %-7.2f %-5d %-7.2f %-14.2f %.2f\n",
            i + 1, p.name, p.price, cart_list[i].quantity,
            cart_list[i].totalWithoutTax, cart_list[i].tax, cart_list[i].totalWithTax);
    }

    // Calculate subtotal and total tax using parallel helper function
    getSubTotalAndTax(cart_list, &totalTax, &subTotal);

    // Print invoice footer with subtotal, tax, discount, and final payable amount
    printf("-------------------------------------------------\n");
    printf("Subtotal: %.2f\n", subTotal);
    printf("Total Tax: %.2f\n", totalTax);

    // Prompt user to enter discount percentage (0-99)
    float discountPercent;
    printf("Enter Discount Percentage (0-99): ");
    scanf("%f", &discountPercent);

    // Calculate discount amount based on subtotal
    float discountAmount = subTotal * (discountPercent / 100.0);
    // Calculate final payable amount (subtotal + tax - discount)
    float finalPayable = subTotal + totalTax - discountAmount;

    // Print discount and final payable amount
    printf("Discount %.2f%% : %.2f\n", discountPercent, discountAmount);
    printf("Final Payable Amount: %.2f\n", finalPayable);
    printf("===================================================================\n");
}
