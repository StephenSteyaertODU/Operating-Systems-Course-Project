# Install WSL for this project, it will make things easier in the long run.

1. Install WSL

Open the Start Menu, type "PowerShell", right-click on Windows PowerShell, and select Run as administrator.
Run the following command:

```bash
wsl --install
```
Restart your computer if prompted.

---

2. Install C++ tools (C++, CMake, Make)

After the restart, open the new Ubuntu app from your Start Menu. You will be prompted to create a Linux username and password

Once your Linux distribution is running, you need to install the C++ compiler (g++ or clang), debugger (gdb), and other essential build tools using the Linux package manager (apt for Ubuntu/Debian)

Open your Ubuntu terminal (from the Windows Start menu).
First, update your package lists:

```bash
sudo apt update
```

Next, install the essential build tools, including g++, gdb, make, and cmake:

```bash
sudo apt install build-essential gdb cmake -y
```

Verify the installation by checking the versions:

```bash
g++ --version
gdb --version
cmake --version
```

---

3. Set Up VS Code for WSL

To have a seamless C++ development experience, it is highly recommended to use Visual Studio Code with the WSL extension.

- Install VS Code on the Windows side

- Open VS Code and install the WSL extension by Microsoft from the Visual Studio Marketplace (extension ID: ms-vscode-remote.remote-wsl).

- You can then use the code . command in your Ubuntu terminal to open a project folder in VS Code, which will automatically connect to the remote WSL environment and provide full editing and debugging support.

---

# Install Git

```bash
sudo apt install git -y
```

Configure Git

```bash
git config --global user.name "Your Name"
git config --global user.email "youremail@domain.com"
```

---

Extensions:

[Click Here](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack) for all of the extensions.

