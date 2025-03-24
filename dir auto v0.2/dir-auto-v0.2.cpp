#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <system_error>
#include <windows.h>
#include <thread>
#include <chrono>

using namespace std;
namespace fs = filesystem;

// 🎨 ฟังก์ชันเปลี่ยนสีข้อความ
void set_color(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 🎭 เอฟเฟกต์การโหลด
void loading_effect()
{
    cout << "🔎 กำลังค้นหา ";
    for (int i = 0; i < 3; i++)
    {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    cout << "\n";
}

// 📂 แสดงโครงสร้างโฟลเดอร์ทีละระดับ
vector<string> list_files(const string &dir)
{
    vector<string> files;
    try
    {
        set_color(14);
        cout << "\n📁 โฟลเดอร์: " << dir << "\n";
        set_color(7);

        int index = 1;
        for (const auto &entry : fs::directory_iterator(dir))
        {
            cout << " " << index++ << ") " << entry.path().filename().string();
            if (fs::is_directory(entry))
                cout << " 📂";
            cout << "\n";

            files.push_back(entry.path().string());
        }
    }
    catch (const fs::filesystem_error &e)
    {
        set_color(12);
        cout << "❌ ข้อผิดพลาด: " << e.what() << "\n";
        set_color(7);
    }
    return files;
}

// 🔍 ค้นหาไฟล์/โฟลเดอร์ว่างเปล่า
vector<string> find_empty(const string &dir)
{
    vector<string> empty_files;
    loading_effect();

    try
    {
        for (const auto &entry : fs::directory_iterator(dir))
        {
            std::error_code ec;
            if (fs::is_empty(entry.path(), ec) && !ec)
            {
                empty_files.push_back(entry.path().string());
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        set_color(12);
        cout << "❌ ข้อผิดพลาด: " << e.what() << "\n";
        set_color(7);
    }
    return empty_files;
}

// 🗑️ ย้ายไฟล์ไปถังขยะ
bool move_to_recycle_bin(const string &path)
{
    SHFILEOPSTRUCTW file_op = {0};
    wstring wpath = wstring(path.begin(), path.end()) + L'\0';
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = wpath.c_str();
    file_op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

    return SHFileOperationW(&file_op) == 0;
}

// 🏗️ เมนูหลัก
void show_menu(const string &path)
{
    int choice;
    cout << "\nเลือกการกระทำสำหรับ \"" << path << "\"\n";
    cout << "1. ลบไฟล์ไปที่ถังขยะ 🗑️\n";
    cout << "2. ค้นหาไฟล์/โฟลเดอร์ว่างเปล่า 🔍\n";
    cout << "3. กลับไป 🔙\n";
    cout << ">> ";

    while (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "โปรดป้อนตัวเลขที่ถูกต้อง: ";
    }

    switch (choice)
    {
    case 1:
        if (move_to_recycle_bin(path))
        {
            set_color(10);
            cout << "✅ \"" << path << "\" ถูกย้ายไปยังถังขยะ\n";
        }
        else
        {
            set_color(12);
            cout << "❌ ไม่สามารถย้าย \"" << path << "\" ไปยังถังขยะได้\n";
        }
        set_color(7);
        break;
    case 2:
        {
            vector<string> empty_files = find_empty(path);
            cout << "\n🔍 พบไฟล์ว่างเปล่าจำนวน " << empty_files.size() << " รายการ:\n";
            for (const string &file : empty_files)
            {
                cout << "📄 " << file << "\n";
            }
        }
        break;
    case 3:
        return;
    default:
        cout << "❌ ตัวเลือกไม่ถูกต้อง\n";
    }
}

// 🚀 โปรแกรมหลัก
int main()
{
    string dir;
    vector<string> previous_dirs;
    int choice;

    do
    {
        set_color(11);
        cout << "\n🔷 ตั้งค่าไดเรกทอรีเริ่มต้น\n";
        set_color(7);
        cout << "1. C:/\n";
        cout << "2. D:/\n";
        cout << "3. E:/\n";
        cout << "4. ระบุโฟลเดอร์เอง\n";
        cout << "5. ค้นหาไฟล์ว่างเปล่าทั้งไดร์ฟ\n";
        cout << "6. ออกจากโปรแกรม ❌\n";
        cout << ">> ";

        while (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "โปรดป้อนตัวเลขที่ถูกต้อง: ";
        }

        switch (choice)
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
            cout << "📂 โปรดป้อนโฟลเดอร์: ";
            cin >> dir;
            if (!fs::exists(dir))
            {
                set_color(12);
                cout << "❌ โฟลเดอร์ \"" << dir << "\" ไม่ถูกต้อง\n";
                set_color(7);
                continue;
            }
            break;
        case 5:
            cout << "📂 โปรดป้อนไดร์ฟที่ต้องการค้นหา (เช่น C:/ หรือ D:/): ";
            cin >> dir;
            if (!fs::exists(dir))
            {
                set_color(12);
                cout << "❌ โฟลเดอร์ \"" << dir << "\" ไม่ถูกต้อง\n";
                set_color(7);
                continue;
            }
            {
                vector<string> empty_files = find_empty(dir);
                cout << "\n🔍 พบไฟล์ว่างเปล่าจำนวน " << empty_files.size() << " รายการ\n";
            }
            continue;
        case 6:
            return 0;
        default:
            cout << "❌ ตัวเลือกไม่ถูกต้อง\n";
            continue;
        }

        vector<string> files = list_files(dir);

        int sub_choice;
        do
        {
            cout << "\n1. เลือกไฟล์/โฟลเดอร์\n";
            cout << "2. ค้นหาไฟล์ว่างเปล่า\n";
            cout << "3. กลับไป\n";
            cout << "4. กลับไปหน้าแรก\n";
            cout << ">> ";

            while (!(cin >> sub_choice))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "โปรดป้อนตัวเลขที่ถูกต้อง: ";
            }

            if (sub_choice == 1)
            {
                int index;
                cout << "📂 เลือกหมายเลขไฟล์/โฟลเดอร์: ";
                cin >> index;

                if (index >= 1 && index <= files.size())
                {
                    string path = files[index - 1];
                    if (fs::is_directory(path))
                    {
                        previous_dirs.push_back(dir);
                        dir = path;
                        files = list_files(dir);
                    }
                    else
                    {
                        show_menu(path);
                    }
                }
                else
                {
                    cout << "❌ ตัวเลือกไม่ถูกต้อง\n";
                }
            }
        } while (sub_choice != 4);

    } while (true);

    return 0;
}
