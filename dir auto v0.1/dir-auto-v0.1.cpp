#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

using namespace std;

// ฟังก์ชันแสดงรายการไฟล์/โฟลเดอร์พร้อมหมายเลข
vector<string> list_files(const string& dir) {
  vector<string> files;
  int index = 1;
  for (const auto& entry : filesystem::directory_iterator(dir)) {
    cout << index++ << ". " << entry.path().string() << endl;
    files.push_back(entry.path().string());
  }
  return files;
}

// ฟังก์ชันดึงที่อยู่ไฟล์/โฟลเดอร์
string get_path(const string& file) {
  return filesystem::absolute(file).string();
}

// ฟังก์ชันลบไฟล์/โฟลเดอร์
bool delete_file_or_folder(const string& path) {
  if (filesystem::is_directory(path)) {
    return filesystem::remove_all(path);
  } else {
    return filesystem::remove(path);
  }
}

// ฟังก์ชันค้นหาและแสดงไฟล์/โฟลเดอร์ว่างเปล่า
void find_and_show_empty(const string& dir) {
  for (const auto& entry : filesystem::recursive_directory_iterator(dir)) {
    if (filesystem::is_empty(entry.path())) {
      cout << "  " << entry.path().string() << endl;
    }
  }
}

// ฟังก์ชันหลัก
int main() {
  // ตั้งค่าไดเร็กทอรีเริ่มต้น
  string dir = "C:/";

  // แสดงรายการไฟล์/โฟลเดอร์
  vector<string> files = list_files(dir);

  // เลือกหมายเลข
  int choice;
  cout << "เลือกไฟล์/โฟลเดอร์ (หมายเลข): ";
  cin >> choice;

  // ตรวจสอบว่าตัวเลือกถูกต้อง
  if (choice < 1 || choice > files.size()) {
    cout << "ตัวเลือกไม่ถูกต้อง\n";
    return 1;
  }

  // ดึงที่อยู่
  string path = get_path(files[choice - 1]);

  // แสดงเมนูเพิ่มเติม
  cout << "\nเลือกการดำเนินการกับ \"" << path << "\":\n";
  cout << "1. ค้นหาและแสดงไฟล์/โฟลเดอร์ว่างเปล่า\n";
  cout << "2. ย้อนกลับ\n";
  cout << ">> ";
  cin >> choice;

  switch (choice) {
    case 1:
      cout << "\nไฟล์/โฟลเดอร์ว่างเปล่า:\n";
      find_and_show_empty(path);
      break;
    case 2:
      break;
    default:
      cout << "ตัวเลือกไม่ถูกต้อง\n";
  }

  return 0;
}
