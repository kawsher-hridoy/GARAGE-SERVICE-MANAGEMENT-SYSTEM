# Garage Service Management System (GSMS)

A console-based garage management application built in C that digitizes and streamlines automotive service operations — from customer registration to billing and revenue reporting.

> **Software Development Capstone Project** | Computer Programming in C

---

## Table of Contents

- [Features](#features)
- [System Architecture](#system-architecture)
- [Tech Stack](#tech-stack)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Data Format](#data-format)
- [Screenshots](#screenshots)
- [Future Enhancements](#future-enhancements)
- [Contributors](#contributors)
- [License](#license)

---

## Features

### Authentication System
- Password-protected login with session management
- Secure logout with session cleanup
- Default credentials: `admin` / `admin`

### Customer & Vehicle Management
- Register customers with integrated vehicle information
- Auto-generated sequential customer IDs (001, 002, ...)
- Search customers by ID
- View all customers in formatted table

### Service Management
- Create service records linked to existing customers
- Status workflow: **Pending** &rarr; **In Progress** &rarr; **Completed**
- Automatic date stamping and cost tracking
- Full service history

### Billing & Invoice System
- Generate invoices from completed services
- Professional formatted receipt printing
- Payment status tracking (Paid / Pending)
- Sequential invoice IDs (INV001, INV002, ...)

### Daily Revenue Reports
- Date-based revenue filtering
- Invoice count and total revenue calculation
- Professional report formatting

---

## System Architecture

```
GSMS
├── Authentication Layer
│   ├── Login / Logout
│   └── Session Management
├── Customer Management
│   ├── Registration (Customer + Vehicle)
│   ├── Search & Display
│   └── File Persistence
├── Service Management
│   ├── Service Creation
│   ├── Status Tracking
│   └── Cost Management
├── Billing Layer
│   ├── Invoice Generation
│   ├── Receipt Printing
│   └── Payment Tracking
└── Reports Layer
    ├── Daily Revenue Reports
    └── Service Analytics
```

---

## Tech Stack

| Component           | Technology                              |
|---------------------|-----------------------------------------|
| Language            | C (ANSI C Standard)                     |
| Platform            | Cross-platform (Windows / Linux / macOS)|
| Data Storage        | File-based (pipe-delimited `.txt`)      |
| User Interface      | Console with ANSI colors & ASCII art    |
| Build               | GCC / any C compiler                    |

---

## Getting Started

### Prerequisites

- A C compiler (GCC recommended)
  ```bash
  # Check if GCC is installed
  gcc --version

  # Install on Ubuntu/Debian
  sudo apt install gcc

  # Install on macOS (with Xcode Command Line Tools)
  xcode-select --install
  ```

### Build & Run

```bash
# Clone the repository
git clone https://github.com/<your-username>/GSMS.git
cd GSMS

# Compile
gcc -o gsms project.c

# Run
./gsms
```

On Windows:
```bash
gcc -o gsms.exe project.c
gsms.exe
```

---

## Usage

1. **Login** with default credentials (`admin` / `admin`)
2. Navigate using the numbered menu:

```
╔══════════════════════════════════════════╗
║         MAIN MENU                        ║
╠══════════════════════════════════════════╣
║  [1] Customer & Vehicle Management       ║
║  [2] Service Management                  ║
║  [3] Billing & Invoices                  ║
║  [4] Reports                             ║
║  [5] Logout                              ║
║  [0] Exit                                ║
╚══════════════════════════════════════════╝
```

3. **Add customers** with their vehicle details
4. **Create services** linked to customers, track status
5. **Generate invoices** from completed services
6. **View daily revenue** reports

---

## Project Structure

```
GSMS/
├── project.c                   # Main application source code (~1,655 lines)
├── data/
│   ├── customers.txt           # Customer & vehicle records
│   ├── services.txt            # Service records
│   └── invoices.txt            # Invoice records
├── docs/
│   ├── Project_Overview.md     # Detailed system overview
│   └── Project_Report.md       # Full project report
├── .gitignore
└── README.md
```

---

## Data Format

All data is stored in pipe-delimited text files under the `data/` directory.

**Customers** (`data/customers.txt`):
```
ID|Name|Phone|Address|Plate|Make|Model|Year|Color
001|John Smith|555-0101|123 Main Street|ABC-1234|Toyota|Camry|2020|Blue
```

**Services** (`data/services.txt`):
```
ServiceID|CustomerID|ServiceName|Date|Status|Cost
001|1|Oil Change|2025-01-15|Completed|50.00
```

**Invoices** (`data/invoices.txt`):
```
InvoiceID|ServiceID|CustomerID|CustomerName|ServiceName|Date|Amount|PaymentStatus
INV001|001|1|John Smith|Oil Change|2025-01-15|50.00|Paid
```

> The `data/` directory is auto-created by the program on first run if it doesn't exist.

---

## Screenshots

<details>
<summary>Click to expand</summary>

> *Add screenshots of the running application here.*
>
> You can capture them by running the program and using your terminal's screenshot feature.

</details>

---

## Future Enhancements

- Database integration (SQLite/MySQL) to replace file-based storage
- Multi-user support with role-based access control
- Password encryption and secure authentication
- Parts inventory management
- Web-based GUI using HTML/CSS/JavaScript
- PDF/Excel report export
- SMS/Email customer notifications

---

## Contributors

| Name | Role |
|------|------|
| **[Your Name]** | Developer |
| **[Team Member Name]** | Developer |

> *Software Development Capstone Project*

---

## License

This project is developed for academic purposes as part of a university capstone course. Feel free to use it as a reference or learning resource.
