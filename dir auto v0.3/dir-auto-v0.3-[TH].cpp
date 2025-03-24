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
            cout << index++ << ". " << entry.path().string() << (filesystem::is_directory(entry) ? " [โฟลเดอร์]" : "") << endl;
            files.push_back(entry.path().string());
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        cout << "เกิดข้อผิดพลาดในการแสดงรายการไฟล์/โฟลเดอร์: " << e.what() << endl;
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
        cout << "เกิดข้อผิดพลาดในการดึงที่อยู่ไฟล์/โฟลเดอร์: " << e.what() << endl;
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
                cout << "ไม่สามารถตรวจสอบ \"" << entry.path().string() << "\": " << ec.message() << "\n";
            }
        }
    }
    catch (const filesystem::filesystem_error &e)
    {
        cout << "เกิดข้อผิดพลาดในการค้นหาไฟล์/โฟลเดอร์ว่างเปล่า: " << e.what() << endl;
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

// ฟังก์ชันแสดงเมนูสำหรับไฟล์/โฟลเดอร์ว่างเปล่า
void show_empty_menu(const vector<string> &empty_files)
{
    int choice;
    cout << "\nเลือกการดำเนินการกับไฟล์/โฟลเดอร์ว่างเปล่า:\n";
    cout << "1. ลบไฟล์/โฟลเดอร์ว่างเปล่าทั้งหมด\n";
    cout << "2. ย้อนกลับ\n";
    cout << ">> ";
    while (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "กรุณากรอกตัวเลขที่ถูกต้อง: ";
    }

    switch (choice)
    {
    case 1:
        for (const string &file : empty_files)
        {
            if (move_to_recycle_bin(file))
            {
                cout << "ย้ายไฟล์ \"" << file << "\" ไปยังถังขยะเรียบร้อย\n";
            }
            else
            {
                cout << "เกิดข้อผิดพลาดในการย้าย \"" << file << "\" ไปยังถังขยะ\n";
            }
        }
        break;
    case 2:
        break;
    default:
        cout << "ตัวเลือกไม่ถูกต้อง\n";
    }
}

// ฟังก์ชันแสดงเมนูเพิ่มเติม
void show_menu(const string &path)
{
    int choice;
    cout << "\nเลือกการดำเนินการกับ \"" << path << "\":\n";
    cout << "1. ลบไฟล์\n";
    cout << "2. ค้นหาไฟล์/โฟลเดอร์ว่างเปล่า\n";
    cout << "3. ย้อนกลับ\n";
    cout << ">> ";
    while (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "กรุณากรอกตัวเลขที่ถูกต้อง: ";
    }

    vector<string> empty_files; // Declare outside the switch

    switch (choice)
    {
    case 1:
        if (move_to_recycle_bin(path))
        {
            cout << "ย้ายไฟล์ \"" << path << "\" ไปยังถังขยะเรียบร้อย\n";
        }
        else
        {
            cout << "เกิดข้อผิดพลาดในการย้าย \"" << path << "\" ไปยังถังขยะ\n";
        }
        break;

    case 2:
        empty_files = find_empty(path);
        cout << "\nค้นหาไฟล์/โฟลเดอร์ว่างเปล่า:\n";
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
        cout << "ตัวเลือกไม่ถูกต้อง\n";
    }
}

int main()
{
    // ตั้งค่าไดเร็กทอรีเริ่มต้น
    int choices;
    string dir;
    vector<string> previous_dirs; // Stack to track previous directories
    do
    {
        cout << "ตั้งค่าไดเร็กทอรีเริ่มต้น:\n";
        cout << "1. C:\n";
        cout << "2. D:\n";
        cout << "3. E:\n";
        cout << "4. อื่นๆ\n";
        cout << "5. สแกนหาไฟล์/โฟลเดอร์ว่างเปล่าทั้งไดร์ฟ\n";
        cout << "6. ออกจากโปรแกรม\n"; // เพิ่มตัวเลือกออกจากโปรแกรม
        cout << ">> ";
        while (!(cin >> choices))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "กรุณากรอกตัวเลขที่ถูกต้อง: ";
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
            cout << "อื่นๆ กรอก: ";
            cin >> dir;
            if (!filesystem::exists(dir))
            {
                cout << "ไดเร็กทอรี \"" << dir << "\" ไม่ถูกต้อง\n";
                continue;
            }
            break;
        case 5:
            {
                cout << "เลือกไดร์ฟที่ต้องการสแกน (เช่น C:/ หรือ D:/): ";
                cin >> dir;
                if (!filesystem::exists(dir))
                {
                    cout << "ไดเร็กทอรี \"" << dir << "\" ไม่ถูกต้อง\n";
                    continue;
                }
                vector<string> empty_files = find_empty(dir);
                cout << "\nค้นหาไฟล์/โฟลเดอร์ว่างเปล่าในไดร์ฟ \"" << dir << "\":\n";
                for (const string &file : empty_files)
                {
                    cout << file << endl;
                }
                show_empty_menu(empty_files);
                continue; // กลับไปที่การตั้งค่าไดเร็กทอรีเริ่มต้น
            }
        case 6:
            return 0; // ออกจากโปรแกรม
        default:
            cout << "ตัวเลือกไม่ถูกต้อง\n";
            continue;
        }

        // แสดงรายการไฟล์/โฟลเดอร์
        vector<string> files = list_files(dir);

        int sub_choice;
        do
        {
            cout << "\nเมนู:\n";
            cout << "ตัวเลือก 1. เลือกไฟล์/โฟลเดอร์ (หมายเลข)\n";
            cout << "ตัวเลือก 2. ค้นหาไฟล์/โฟลเดอร์ว่างเปล่า\n";
            cout << "ตัวเลือก 3. ย้อนกลับ\n";
            cout << "ตัวเลือก 4. กลับไปยังการตั้งค่าไดเร็กทอรีเริ่มต้น\n";
            cout << ">> ";
            while (!(cin >> sub_choice))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "กรุณากรอกตัวเลขที่ถูกต้อง: ";
            }

            switch (sub_choice)
            {
            case 1:
                {
                    int choice;
                    cout << "เลือกไฟล์/โฟลเดอร์ (หมายเลข): ";
                    cin >> choice;

                    if (choice >= 1 && choice <= files.size())
                    {
                        string path = get_path(files[choice - 1]);
                        if (filesystem::is_directory(path))
                        {
                            previous_dirs.push_back(dir); // บันทึก directory ปัจจุบันก่อนเปลี่ยน
                            dir = path;
                            files = list_files(dir); // อัปเดตรายการไฟล์/โฟลเดอร์
                        }
                        else
                        {
                            show_menu(path);
                        }
                    }
                    else
                    {
                        cout << "ตัวเลือกไม่ถูกต้อง\n";
                    }
                }
                break;

            case 2:
                {
                    vector<string> empty_files = find_empty(dir);
                    cout << "\nค้นหาไฟล์/โฟลเดอร์ว่างเปล่าใน \"" << dir << "\":\n";
                    for (const string &file : empty_files)
                    {
                        cout << file << endl;
                    }
                    show_empty_menu(empty_files);
                }
                break;

            case 3:
                if (!previous_dirs.empty())
                {
                    dir = previous_dirs.back();
                    previous_dirs.pop_back();
                    files = list_files(dir);
                }
                else
                {
                    cout << "ไม่มี directory ก่อนหน้า\n";
                }
                break;

            case 4:
                previous_dirs.clear(); // ล้าง stack เพื่อกลับไปยังการตั้งค่าเริ่มต้น
                goto reset_directory;
                break;

            default:
                cout << "ตัวเลือกไม่ถูกต้อง\n";
            }
        } while (sub_choice != 4);

    reset_directory:
        continue; // กลับไปที่การตั้งค่าไดเร็กทอรีเริ่มต้น
    } while (true);

    return 0;
}