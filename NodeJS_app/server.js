import { createServer } from 'http';
import { parse } from 'url'; 
import { WebSocketServer } from 'ws';
import { readFile } from 'node:fs';

const HttpServer = createServer((request, response) => {
    response.writeHead(200, { 'Content-Type': 'text/html' });
    readFile('./index.html', null, function (error, data) {
        if (error) {
            response.writeHead(404);
            response.write('File not found!');
        } else {
            response.write(data);
        }
        response.end();
    });
});

HttpServer.on('upgrade', function upgrade(request, socket, head) {
    const { pathname } = parse(request.url);
    if (pathname === '/socket1') {
        wss1.handleUpgrade(request, socket, head, function done(ws) {
        wss1.emit('connection', ws, request);
      });
    }else if (pathname === '/socket2') {
        wss2.handleUpgrade(request, socket, head, function done(ws) {
        wss2.emit('connection', ws, request);
      });
    }else {
        socket.destroy();
    }
});

const wss1 = new WebSocketServer({ noServer: true });
const wss2 = new WebSocketServer({ noServer: true });

wss1.on('connection', function connection(ws) {
    console.log('ws1 client connected');
    ws.on('message', function(data) {
        wss1.clients.forEach(function each(client) {
            client.send(binariosA64(data));
        });
    });
});
wss2.on('connection', function connection(ws) {
    console.log('ws2 client connected');
    ws.on('message', function (data) {
        wss2.clients.forEach(function each(client) {
            client.send(bufToString(data));
        });
    });
});

function bufToString(data){
    var buf = Buffer.from(data);
    var datos = buf.toString();
    return datos;
}
function binariosA64(data){
    var u8 = new Uint8Array(data);
    var b64 = Buffer.from(u8).toString('base64');
    return b64;
}

HttpServer.listen(3000);



