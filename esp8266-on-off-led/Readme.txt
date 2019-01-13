https://electrosome.com/led-control-esp8266-web-server/

1. Upload source vào bo mạch
- Chọn boar: NodeMCU 0.9 (ESP 12 - Module)
- Chọn cổng COM tương ứng, nếu chưa update driver cho cổng COM thì arduino sẽ ko nhận cổng com, vào Device Manager để update driver
	driver chứa trong thư mục arduino library (hình như thế)

2. Tiến hành lắp bo mạch
Lưu ý: sẽ có 2 chân ra là D0 và GND, D0 sẽ nối với chân dài của đèn led (+) còn GND nối với chân ngắn. khỏi cần điện trỡ cũng dc

3. Điều khiển qua web server
- trường hợp khi restart là vẫn ko nhìn thấy ip của cục esp8266 thì tiến hành tìm ip bằng command line