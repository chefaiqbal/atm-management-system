# 🏧 ATM Operations/Management Project

Welcome to the **ATM Operations/Management** project! 🚀 This application simulates basic ATM functionalities, allowing users to perform transactions seamlessly.

## 📦 Table of Contents
- [Features](#features)
- [Database Operations](#database-operations)
- [Modules](#modules)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Data Security](#data-security)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## ✨ Features
- **User Authentication**: Secure login system to protect user data.
- **Balance Inquiry**: Check your current account balance.
- **Cash Withdrawal**: Withdraw cash with ease.
- **Deposit Funds**: Add money to your account.
- **Transaction History**: View your past transactions.
- **Interest Calculation**: Automatically calculate and apply interest to savings and fixed accounts.
- **Account Management**: Create, update, and delete accounts with ease.
- **User Registration**: New users can register and create an account.
- **Ownership Transfer**: Transfer account ownership to another user.
- **Secure Password Handling**: Passwords are hashed using SHA256 for security.
- **Error Handling**: Robust error handling for database operations and user inputs.

## 🗄 Database Operations
- **CRUD Operations**: Create, Read, Update, and Delete data seamlessly.
- **Secure Data Storage**: Protect sensitive information with encryption.
- **Efficient Queries**: Optimize data retrieval with advanced SQL queries.

## 🧩 Modules
- **Authentication Module**: Handles user login and registration.
- **ATM Operations Module**: Manages transactions like deposits, withdrawals, and balance inquiries.
- **Database Operations Module**: Interfaces with the SQLite database to perform CRUD operations.

## 🛠 Installation
1. **Clone the repository**
   ```bash
   git clone https://github.com/chefaiqbal/atm-operations.git
   ```
2. **Navigate to the project directory**
   ```bash
   cd atm-operations
   ```
3. **Build the project using Makefile**
   ```bash
   make
   ```

## 🚀 Usage
1. **Run the application**
   ```bash
   ./atm_system
   ```
2. **Follow the on-screen prompts to perform transactions**

## 🔧 Configuration
- **Database Initialization**: The database is initialized and tables are created if they do not exist.
- **Environment Setup**: Ensure the data directory exists for storing the SQLite database.

## 📊 Data Security
- **Password Encryption**: User passwords are securely hashed before storage.
- **Data Validation**: Input validation for user data such as phone numbers and account types.

## 🤝 Contributing
Contributions are welcome! Please fork the repository and submit a pull request for any enhancements or bug fixes.

## 📄 License
This project is licensed under the [MIT License](LICENSE).

## 📫 Contact
- **Email**: amiriqbal87@gmail.com
- **GitHub**: [chefaiqbal](https://github.com/chefaiqbal)
