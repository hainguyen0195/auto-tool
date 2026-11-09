# Vocal Live Studio

**Vocal Live Studio** là ứng dụng xử lý giọng hát trực tiếp (Real-Time Vocal Studio) chuyên nghiệp dành cho Windows 10/11 64-bit, được xây dựng trên nền tảng **C++17** và framework âm thanh chuẩn công nghiệp **JUCE**.

![Vocal Live Studio](Resources/app_icon.ico)

---

## 1. Giới thiệu tổng quan

Vocal Live Studio cung cấp giải pháp xử lý giọng hát trực tiếp độ trễ cực thấp cho các nhu cầu:
- **Hát Karaoke chuyên nghiệp**: Tích hợp trình phát nhạc nền MP3/WAV với khả năng tăng giảm tone (Transpose $\pm 6$ semitones) và tốc độ (Tempo).
- **Auto-Tune cao cấp**: Tự động nhận diện cao độ thực tế bằng thuật toán YIN chuẩn xác, hiệu chỉnh pitch mượt mà theo Key và Scale.
  - Chế độ **NATURAL**: Giữ trọn vibrato, biểu cảm tự nhiên, hoàn hảo cho Bolero, Ballad, nhạc trữ tình.
  - Chế độ **HARD TUNE**: Bắt nốt tức thì, hiệu ứng điện tử chuẩn Pop, Trap, EDM.
- **Rack hiệu ứng Vocal đầy đủ**: Noise Gate, De-Esser, 3-Band Parametric EQ, Studio Compressor, Ping-Pong Delay, Hall Reverb, Anti-Clipping Limiter.
- **Livestream trực tiếp**: Xuất âm thanh đã qua xử lý sang OBS Studio, TikTok Live Studio, Bigo Live, v.v. thông qua Virtual Audio Routing.

---

## 2. Yêu cầu hệ thống

| Thành phần | Yêu cầu tối thiểu | Khuyến nghị |
| :--- | :--- | :--- |
| **Hệ điều hành** | Windows 10/11 64-bit | Windows 11 64-bit |
| **CPU** | Intel Core i5 / AMD Ryzen 5 | Intel Core i7 / AMD Ryzen 7 |
| **RAM** | 8 GB | 16 GB hoặc cao hơn |
| **Audio Device** | Microphone tích hợp hoặc USB | Soundcard USB / Audio Interface ASIO |
| **Tai nghe** | Headphone 3.5mm hoặc USB | Headphone có dây để hạn chế trễ |

---

## 3. Các tính năng cốt lõi

### A. Real-Time Audio Engine
- Tương thích driver WASAPI (Shared & Exclusive) và ASIO.
- Hỗ trợ Sample Rate 44.1 kHz và 48.0 kHz.
- Buffer Size tùy chọn: 64, 128, 256, 512 samples.
- Điều khiển Mic Gain và Master Output Volume với Peak Meter thời gian thực.
- Tùy chọn **Headphone Monitor** (Bật/Tắt nghe lại giọng của mình qua tai nghe).
- **Safe Mode**: Chống tràn bộ đệm khi CPU quá tải.
- Kiến trúc xử lý Real-Time Safe: Không cấp phát bộ nhớ (`malloc`/`new`), không thao tác File I/O hay gọi giao diện trong Audio Callback thread.

### B. Bộ hiệu chỉnh cao độ Auto-Tune
- **Pitch Detection (YIN Algorithm)**:
  - Dò tần số cơ bản $f_0$ trong dải tần số giọng hát 60 Hz – 1100 Hz.
  - Đánh giá độ tin cậy chu kỳ (Voiced Confidence) và mức năng lượng RMS để loại bỏ tạp âm, tiếng thở và phụ âm xì.
  - Lọc trung vị (3-point median filter) chống hiện tượng nhảy quãng tám.
- **Note Quantization**:
  - Hỗ trợ đầy đủ 12 Root Key: C, C#, D, D#, E, F, F#, G, G#, A, A#, B.
  - Các Scale thông dụng: Chromatic, Major, Natural Minor, Harmonic Minor, Pentatonic Major, Pentatonic Minor, và **Custom Scale** (cho phép bật/tắt từng nốt trong quãng 8).
- **Tham số điều khiển**:
  - **Retune Speed (0 - 300 ms)**: Thời gian lướt đến nốt đích.
  - **Correction Strength (0 - 100%)**: Mức độ kéo nốt.
  - **Humanize (0 - 100%)**: Tạo vùng an toàn cho vibrato tự nhiên không bị gò ép.

### C. Bộ hiệu ứng Vocal Effects Rack
1. **Noise Gate**: Loại bỏ tiếng ồn nền, quạt máy tính, tiếng hú môi trường khi không hát.
2. **De-Esser**: Tinh chỉnh tần số 4 - 8 kHz, giảm chói gắt các âm "x", "s", "ch".
3. **3-Band Parametric EQ**: Bù âm trầm (Low Shelf), làm nổi bật giọng hát (Mid Bell), tạo độ sáng bay bổng (High Shelf).
4. **Vocal Compressor**: Nén dải động mượt mà với Soft-Knee, làm dày giọng và đồng đều âm lượng.
5. **Echo / Delay**: Ping-pong stereo delay có bộ lọc damp tần số cao cho hiệu ứng vang ấm áp.
6. **Studio Reverb**: Không gian phòng thu / hội trường ảo chuẩn Stereo, tôn giọng ngọt ngào.
7. **Anti-Clipping Limiter**: Giới hạn đỉnh âm thanh ở -0.2 dBFS, bảo vệ loa và tai nghe khỏi hiện tượng méo tiếng số (digital clipping).

### D. Trình phát Karaoke & Backing Track
- Hỗ trợ định dạng âm thanh: MP3, WAV, FLAC, AIFF.
- Đồng bộ phát nhạc nền với giọng hát micro qua bus Master.
- Tùy chỉnh Key Transpose ($\pm 6$ nửa cung) để phù hợp tone giọng người hát.
- Tùy chỉnh tốc độ bài hát (Tempo 80% – 120%).
- Thanh trượt vị trí bài hát (Seek bar) mượt mà.

### E. Quản lý cấu hình (Presets)
Tích hợp sẵn 8 Preset tiêu chuẩn phòng thu:
1. **Bolero Warm**: Âm sắc trầm ấm, Reverb hội trường sâu, Auto-tune tự nhiên, tôn giọng hát tình cảm.
2. **Bolero Sweet**: Tươi sáng mượt mà, Humanize cao giữ trọn rung ngân, slapback delay ngọt ngào.
3. **Pop Natural**: Giọng pop hiện đại, compressor nhạy, delay và reverb hiện đại.
4. **Pop Hard Tune**: Auto-tune bám nốt tức thì, hiệu ứng robotic đặc trưng cho nhạc trẻ sôi động.
5. **EDM Electronic**: Hard tune 100%, không gian rộng, phù hợp nhạc điện tử và dance.
6. **Male Deep**: Tối ưu dải trầm cho giọng nam, cắt bỏ ù rền tần số thấp.
7. **Female Soft**: Nâng dải bay bổng, kích hoạt de-esser chống gắt, giọng nữ trong trẻo.
8. **Karaoke Clean**: Giọng mộc tinh tế, cân bằng cho hát live tại gia hoặc sân khấu nhỏ.
- Hỗ trợ **Save** và **Load** preset tùy chỉnh ra file JSON.

---

## 4. Hướng dẫn kết nối Livestream (OBS / TikTok Live)

Để truyền tín hiệu âm thanh đã qua xử lý Auto-Tune và Reverb sang phần mềm Livestream mà không bị lặp âm (echo):

1. **Cài đặt thiết bị âm thanh ảo**:
   - Tải và cài đặt miễn phí **VB-Audio Virtual Cable** (driver `VB-CABLE`).
2. **Cấu hình trên Vocal Live Studio**:
   - Bấm nút **[Audio Setup]** ở thanh trên cùng.
   - Phần **Audio Output Device**: Chọn **CABLE Input (VB-Audio Virtual Cable)**.
   - Đảm bảo Microphone thật của bạn được chọn ở phần **Audio Input Device**.
3. **Cấu hình trên OBS Studio**:
   - Mở OBS Studio -> **Settings** -> **Audio**.
   - Tại mục **Mic/Auxiliary Audio**, chọn **CABLE Output (VB-Audio Virtual Cable)**.
   - Nhấn OK. Toàn bộ giọng hát kèm nhạc nền sẽ phát trực tiếp vào livestream với chất lượng chuẩn phòng thu!
4. **Cấu hình trên TikTok Live Studio**:
   - Mở TikTok Live Studio -> **Cài đặt âm thanh**.
   - Chọn Microphone đầu vào là **CABLE Output**.

---

## 5. Hướng dẫn biên dịch từ Source Code

### Công cụ yêu cầu:
- Windows 10/11 64-bit
- CMake 3.22 trở lên
- Trình biên dịch C++17 (MSVC Visual Studio 2022 hoặc GCC / MinGW-w64 64-bit)
- Ninja build system
- Inno Setup 6 hoặc 7 (để đóng gói installer)

### Các bước biên dịch:
```powershell
# 1. Chuyển vào thư mục dự án
cd D:\auto-tool\VocalLiveStudio

# 2. Cấu hình CMake với Release x64
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release

# 3. Biên dịch mã nguồn thành file EXE
cmake --build build --config Release

# 4. Đóng gói bộ cài đặt Setup
& "C:\Program Files\Inno Setup 7\ISCC.exe" Installer\installer.iss
```

Sau khi hoàn tất, file cài đặt sẽ được tạo tại:
`D:\auto-tool\VocalLiveStudio\bin\VocalLiveStudio_Setup.exe`

---

## 6. Giấy phép

Xem chi tiết trong tệp [LICENSES.md](LICENSES.md).
Phát triển và phân phối bởi Vocal Live Studio Team.
