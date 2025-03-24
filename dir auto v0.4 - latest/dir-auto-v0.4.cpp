#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <system_error>
#include <windows.h>

using namespace std;

// ฟังก์ชันแสดงรายการไฟล์/โฟลเดอร์พร้อมหมายเลข
vector<string> list_files(const string &dir)
{
    vector<string> files;
    try
    {
        int index = 1;
        for (const auto &entry : filesystem::directory_iterator(dir))
        {
            cout << index++ << ". " << entry.path().string() << (filesystem::is_directory(entry) ? " [Folder]" : "") << endl;
            files.push_back(entry.path().string());
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        cout << "Error listing files/folders: " << e.what() << endl;
    }
    return files;
}

// ฟังก์ชันดึงที่อยู่ไฟล์/โฟลเดอร์
string get_path(const string &file)
{
    try
    {
        return filesystem::absolute(file).string();
    }
    catch (const filesystem::filesystem_error &e)
    {
        cout << "Error retrieving file/folder path: " << e.what() << endl;
        return "";
    }
}

// ฟังก์ชันค้นหาไฟล์/โฟลเดอร์ว่างเปล่า
vector<string> find_empty(const string &dir)
{
    vector<string> empty_files;
    try
    {
        for (const auto &entry : filesystem::directory_iterator(dir))
        {
            std::error_code ec;
            if (filesystem::is_empty(entry.path(), ec)) // ใช้ error_code เพื่อตรวจสอบข้อผิดพลาด
            {
                if (!ec) // ตรวจสอบว่าไม่มีข้อผิดพลาด
                {
                    empty_files.push_back(entry.path().string());
                }
            }
            else if (ec) // หากเกิดข้อผิดพลาด
            {
                cout << "Cannot check \"" << entry.path().string() << "\": " << ec.message() << "\n";
            }
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        cout << "Error finding empty files/folders: " << e.what() << endl;
    }
    return empty_files;
}

// ฟังก์ชันแปลง string เป็น wstring
std::wstring string_to_wstring(const std::string &str)
{
    return std::wstring(str.begin(), str.end());
}

// ฟังก์ชันลบไฟล์/โฟลเดอร์ไปยังถังขยะ
bool move_to_recycle_bin(const string &path)
{
    SHFILEOPSTRUCTW file_op = {0};
    std::wstring wpath_with_null = string_to_wstring(path) + L'\0'; // Add double null-termination
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = wpath_with_null.c_str();
    file_op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

    return SHFileOperationW(&file_op) == 0;
}

// ฟังก์ชันลบไฟล์หรือโฟลเดอร์ว่างเปล่าและโฟลเดอร์ย่อยๆ ที่ว่างเปล่าให้อัตโนมัติ
void delete_empty_recursive(const string &dir)
{
    try
    {
        for (const auto &entry : filesystem::directory_iterator(dir))
        {
            if (filesystem::is_directory(entry))
            {
                delete_empty_recursive(entry.path().string()); // Recursive call for subdirectories
            }

            std::error_code ec;
            if (filesystem::is_empty(entry.path(), ec) && !ec)
            {
                if (move_to_recycle_bin(entry.path().string()))
                {
                    cout << "Moved empty \"" << entry.path().string() << "\" to the recycle bin\n";
                }
                else
                {
                    cout << "Error moving \"" << entry.path().string() << "\" to the recycle bin\n";
                }
            }
            else if (ec)
            {
                cout << "Cannot check \"" << entry.path().string() << "\": " << ec.message() << "\n";
            }
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        cout << "Error processing directory \"" << dir << "\": " << e.what() << endl;
    }
}

// ฟังก์ชันแสดงเมนูสำหรับไฟล์/โฟลเดอร์ว่างเปล่า
void show_empty_menu(const vector<string> &empty_files)
{
    int choice;
    string dir;
    vector<string> previous_dirs; // Stack to track previous directories

    cout << "\nChoose action for empty files/folders:\n";
    cout << "1. Delete all empty files/folders\n";
    cout << "2. Go back\n";
    cout << ">> ";
    while (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a valid number: ";
    }

    vector<string> files = list_files(dir);

    switch (choice)
    {
    case 1:
        for (const string &file : empty_files)
        {
            if (move_to_recycle_bin(file))
            {
                cout << "Moved file \"" << file << "\" to the recycle bin\n";
                if (!previous_dirs.empty())
                {
                    dir = previous_dirs.back();
                    previous_dirs.pop_back();
                    files = list_files(dir);
                }
                else
                {
                    cout << "No previous directory\n";
                }
            }
            else
            {
                cout << "Error moving \"" << file << "\" to the recycle bin\n";
            }
        }
        break;
    case 2:
        break;
    default:
        cout << "Invalid option\n";
    }
}

// ฟังก์ชันแสดงเมนูเพิ่มเติม
void show_menu(const string &path)
{
    int choice;
    string dir;
    vector<string> previous_dirs; // Stack to track previous directories

    cout << "\nChoose action for \"" << path << "\":\n";
    cout << "1. Move file to recycle bin\n";
    cout << "2. Find empty files/folders\n";
    cout << "3. Go back\n";
    cout << ">> ";
    while (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a valid number: ";
    }

    vector<string> empty_files; // Declare outside the switch
    vector<string> files = list_files(dir);

    switch (choice)
    {
    case 1:
        if (move_to_recycle_bin(path))
        {
            cout << "Moved file \"" << path << "\" to the recycle bin\n";
            if (!previous_dirs.empty())
            {
                dir = previous_dirs.back();
                previous_dirs.pop_back();
                files = list_files(dir);
            }
            else
            {
                cout << "No previous directory\n";
            }
        }
        else
        {
            cout << "Error moving \"" << path << "\" to the recycle bin\n";
        }
        break;

    case 2:
        empty_files = find_empty(path);
        cout << "\nFind empty files/folders:\n";
        for (const string &file : empty_files)
        {
            cout << file << endl;
        }

        // แสดงเมนูสำหรับไฟล์/โฟลเดอร์ว่างเปล่า
        show_empty_menu(empty_files);
        break;

    case 3:
        break;
    default:
        cout << "Invalid option\n";
    }
}

// ฟังก์ชันขั้นสูงโดยจะเป็นการค้นหาไฟล์หรือโฟลเดอร์ว่างเปล่าในไดรฟ์ที่เลือก จากนั้นทำการลบไฟล์หรือโฟลเดอร์ และไฟล์หรือโฟลเดอร์ย่อยๆ ที่ว่างเปล่าให้อัตโนมัติ

int main()
{
    // ตั้งค่าไดเร็กทอรีเริ่มต้น
    int choices;
    string dir;
    vector<string> previous_dirs; // Stack to track previous directories
    do
    {
        cout << "Set initial directory:\n";
        cout << "1. C:\n";
        cout << "2. D:\n";
        cout << "3. E:\n";
        cout << "4. Other...\n";
        cout << "5. Scan empty files/folders across drive\n";
        cout << "6. Delete empty files/folders recursively\n"; // New menu option
        cout << "7. Exit program\n";                           // Adjusted exit option number
        cout << ">> ";
        while (!(cin >> choices))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a valid number: ";
        }
        cout << "\n";

        switch (choices)
        {
        case 1:
            dir = "C:/";
            break;
        case 2:
            dir = "D:/";
            break;
        case 3:
            dir = "E:/";
            break;
        case 4:
            cout << "Other... Enter directory: ";
            cin >> dir;
            if (!filesystem::exists(dir))
            {
                cout << "Directory \"" << dir << "\" is not valid\n";
                continue;
            }
            break;
        case 5:
        {
            cout << "Select drive to scan (e.g., C:/ or D:/): ";
            cin >> dir;
            if (!filesystem::exists(dir))
            {
                cout << "Directory \"" << dir << "\" is not valid\n";
                continue;
            }
            vector<string> empty_files = find_empty(dir);
            cout << "\nFinding empty files/folders in drive \"" << dir << "\":\n";
            for (const string &file : empty_files)
            {
                cout << file << endl;
            }
            show_empty_menu(empty_files);
            continue; // Back to initial directory setting
        }
        case 6:
        {
            cout << "Enter path to scan and delete empty files/folders (e.g., C:/, D:/, or E:/path/your-folder): ";
            cin.ignore();      // Clear input buffer
            getline(cin, dir); // Allow spaces in the path
            if (!filesystem::exists(dir))
            {
                cout << "Directory \"" << dir << "\" is not valid\n";
                continue;
            }
            cout << "Deleting empty files/folders in \"" << dir << "\":\n";
            delete_empty_recursive(dir);
            continue; // Back to initial directory setting
        }
        case 7:
            return 0; // Exit program
        default:
            cout << "Invalid option\n";
            continue;
        }

        // Show list of files/folders
        vector<string> files = list_files(dir);

        int sub_choice;
        do
        {
            cout << "\nMenu:\n";
            cout << "Option 1. Choose file/folder (number)\n";
            cout << "Option 2. Find empty files/folders\n";
            cout << "Option 3. Move file\n";
            cout << "Option 4. Go back\n";
            cout << "Option 5. Return to initial directory setting\n";
            cout << ">> ";
            while (!(cin >> sub_choice))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a valid number: ";
            }

            int choice;
            string recheck;

            switch (sub_choice)
            {
            case 1:
            {
                cout << "Choose file/folder (number): ";
                cin >> choice;

                if (choice >= 1 && choice <= files.size())
                {
                    string path = get_path(files[choice - 1]);
                    if (filesystem::is_directory(path))
                    {
                        previous_dirs.push_back(dir); // Save current directory before changing
                        dir = path;
                        files = list_files(dir); // Update file/folder list
                    }
                    else
                    {
                        show_menu(path);
                    }
                }
                else
                {
                    cout << "Invalid option\n";
                }
            }
            break;

            case 2:
            {
                vector<string> empty_files = find_empty(dir);
                cout << "\nFinding empty files/folders in \"" << dir << "\":\n";
                for (const string &file : empty_files)
                {
                    cout << file << endl;
                }
                show_empty_menu(empty_files);
            }
            break;

            case 3:

                cout << "Choose file/folder (number): ";
                cin >> choice;

                if (choice >= 1 && choice <= files.size())
                {
                    string path = get_path(files[choice - 1]);
                    cout << "Are you sure you want to move this file/folder to the recycle bin? (Y/N): ";
                    cin >> recheck;

                    if (recheck == "Y" || recheck == "y")
                    {
                        // if (move_to_recycle_bin(path))
                        // {
                        //     cout << "Moved file \"" << path << "\" to the recycle bin\n";
                        //     if (!previous_dirs.empty())
                        //     {
                        //         dir = previous_dirs.back();
                        //         previous_dirs.pop_back();
                        //         files = list_files(dir);
                        //     }
                        //     else
                        //     {
                        //         cout << "No previous directory\n";
                        //     }
                        // }
                        // else
                        // {
                        //     cout << "Error moving \"" << path << "\" to the recycle bin\n";
                        // }

                        // cout << "Enter path to scan and delete empty files/folders (e.g., C:/, D:/, or E:/path/your-folder): ";
                        // cin.ignore();      // Clear input buffer
                        // getline(cin, dir); // Allow spaces in the path
                        // if (!filesystem::exists(dir))
                        // {
                        //     cout << "Directory \"" << dir << "\" is not valid\n";
                        //     continue;
                        // }
                        // cout << "Deleting empty files/folders in \"" << dir << "\":\n";
                        // delete_empty_recursive(dir);
                        // continue; // Back to initial directory setting

                        if (!filesystem::exists(path))
                        {
                            cout << "Directory \"" << path << "\" is not valid\n";
                            continue;
                        }
                        cout << "Deleting empty files/folders in \"" << path << "\":\n";
                        delete_empty_recursive(path);
                        continue; // Back to initial directory setting
                    }
                    else
                    {
                        cout << "Operation cancelled\n";
                    }
                    break;
                }
                else
                {
                    cout << "Invalid option\n";
                }
                break;

            case 4:
                if (!previous_dirs.empty())
                {
                    dir = previous_dirs.back();
                    previous_dirs.pop_back();
                    files = list_files(dir);
                }
                else
                {
                    cout << "No previous directory\n";
                }
                break;

            case 5:
                previous_dirs.clear(); // Clear stack to return to initial setting
                goto reset_directory;
                break;

            default:
                cout << "Invalid option\n";
            }
        } while (sub_choice != 5);

    reset_directory:
        continue; // Back to initial directory setting
    } while (true);

    return 0;
}
