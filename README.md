# 使用霍夫曼編碼對圖檔bmp壓縮
使用了lenna當作主題將圖片壓縮與解壓縮
## 使用encoder對影像lenna.bmp進行編碼，儲存編碼後位元串於磁碟中，並計算編碼位元率 (bits per pixel)？
根據壓縮圖檔後儲存的的txt檔有1,923KB=15,746,736bit，此lena為512×512 8bit的灰階圖片，15,746,736÷512×512=60.06bpp
## 使用decoder對編碼後位元串進行解碼，還原影像為lenna_r.bmp，並計算lenna與lenna_r兩張影像的MSE為何？
因為是無失真壓縮MSE為0
## 請將上述的超作與計算所用的程式碼，及計算結果與過程錄影紀錄，上傳至youtube，提供該錄影紀錄之URL至ulearn作業區批改。
https://youtu.be/oheJkTubxDw
