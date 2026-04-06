# Typing
# 💻 Người Que Gõ Máy Tính - Matrix Typing

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus" alt="C++17">
  <img src="https://img.shields.io/badge/SFML-2.6-green?logo=sfml" alt="SFML">
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey" alt="Platform">
  <img src="https://img.shields.io/badge/License-MIT-yellow" alt="MIT License">
</p>

<p align="center">
  <i>✨ Một chương trình gõ phím tương tác với nhân vật người que sống động, hiệu ứng Matrix và chế độ Bug đầy bất ngờ ✨</i>
</p>

---

## 🎮 Giới thiệu

**Người Que Gõ Máy Tính** là một ứng dụng giải trí kết hợp giữa luyện gõ phím và hiệu ứng thị giác vui nhộn. Bạn sẽ điều khiển một người que ngồi trước máy tính, gõ phím và xem những phản ứng hài hước:

- 💨 **Gõ nhanh** → khói bốc lên từ bàn phím
- 😭 **Gõ sai chính tả** → nước mắt bắn ra
- 🐞 **Chế độ Bug** → mưa ký tự lỗi, nổ tung màn hình
- 💥 **Đập bàn phím** → rung lắc, vỡ phím
- 🎨 **Hiệu ứng Matrix** – chữ xanh neon trên nền đen

---

## 📸 Giao diện & Tính năng

| Tính năng | Mô tả |
|-----------|-------|
| 🧍 **Người que** | Ngồi đối diện bàn phím, hai tay di chuyển linh hoạt khi gõ |
| 🖥️ **Màn hình CRT** | Khung viền xanh, hiển thị chữ với hiệu ứng con trỏ nhấp nháy |
| ⌨️ **Bàn phím ảo** | Các phím sáng lên mỗi lần gõ |
| ✍️ **Soạn thảo** | Tự động xuống dòng, cuộn khi đầy màn hình |
| 🔥 **Gõ nhanh** | >3 phím/0.2 giây → khói bốc lên |
| 😢 **Sai chính tả** | Từ khóa "matrix" – gõ sai sẽ có nước mắt |
| 💥 **Bug Mode (B)** | Mưa ký tự lỗi `! @ # $ % { } ; ?` + nổ đỏ khắp màn hình |
| 🎯 **Đập bàn phím (C+Space)** | Rung màn hình, vỡ phím, mất nửa số chữ |

---

## 🛠️ Công nghệ sử dụng

| Thành phần | Công nghệ |
|------------|-----------|
| **Ngôn ngữ** | C++17 |
| **Đồ họa & Âm thanh** | SFML 2.6 |
| **Hệ thống Particle** | Tự phát triển (khói, nước mắt, nổ, mảnh vỡ) |
| **Xử lý văn bản** | TextBuffer – tự động xuống dòng, giới hạn vùng hiển thị |
| **Hiệu ứng** | Rung màn hình, glow bàn phím, animation tay |

---

## 📥 Cài đặt & Chạy chương trình

### Yêu cầu hệ thống
- **Windows**: Code::Blocks + SFML
- **Linux**: g++ + SFML
- **macOS**: Clang + SFML (có thể cần điều chỉnh)

### Hướng dẫn nhanh (Code::Blocks)

1. **Tạo project SFML mới**
2. **Copy toàn bộ code** vào file `main.cpp`
3. **Build & Run** (F9)

> ⚠️ Đảm bảo file `arial.ttf` có trong thư mục debug/release (hoặc sửa font trong code)

---

## 🎮 Hướng dẫn sử dụng

| Phím | Chức năng |
|------|-----------|
| **A-Z, 0-9, dấu câu** | Gõ chữ (tay người que cử động) |
| **Backspace** | Xóa 1 ký tự |
| **Delete** | Xóa toàn bộ văn bản |
| **B** | Bật/tắt **Bug Mode** (nổ đỏ 💥) |
| **C + Space** | Đập bàn phím mạnh |
| **ESC** | Thoát chương trình |

---

## 🧩 Cấu trúc code chính
[DEMO]

[Trở về trang chủ](https://github.com/roboeinlife-glitch)
