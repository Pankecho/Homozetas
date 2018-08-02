const Express = require('express');
const SocketIO = require('socket.io');
const SerialPort = require('serialport');
const Http = require('http');
const Cors = require('cors');
const BodyParser = require('body-parser');

const app = Express();

app.use(Express.static(__dirname + "/public"));
app.use(Cors());
app.use(BodyParser.json());
app.use(BodyParser.urlencoded({ extended: true }));

const server = Http.createServer(app);
const io = SocketIO.listen(server);

io.on('connect', socket =>{
	console.log("Se conectó un nuevo socket");
});

app.get('/',(req, res, next)=>{
	res.sendFile(__dirname + "/index.html");
});
app.get('/configuracion',(req, res, next)=>{
	res.sendFile(__dirname + "/configuracion.html");
});

app.post('/riego',(req, res, next) =>{
    mySerial.write('RIEGO');
    res.status(200).end();
});
app.post('/calefaccion',(req, res, next) =>{
    mySerial.write('CALEFACCION');
    res.status(200).end();
});
app.post('/goteo',(req, res, next) =>{
    mySerial.write('GOTEO');
    res.status(200).end();
});
app.post('/foco',(req, res, next) =>{
    mySerial.write('FOCO');
    res.status(200).end();
});
app.post('/configuracion',(req, res, next)=>{
	console.log(req.body);
	const peticion = req.body;
	if(parseInt(peticion.tiempoRiego)){
		const tiempo = parseInt(peticion.tiempoRiego);
		const intervalo = peticion.intervaloR;
		mySerial.write(`RIEGO: ${tiempo} ${intervalo}`);
	}
	if(parseInt(peticion.tiempoFoco)){
		const tiempo = parseInt(peticion.tiempoFoco);
		const intervalo = peticion.intervalo;
		mySerial.write(`FOCO: ${tiempo} ${intervaloF}`);
	}
	if(parseInt(peticion.temperaturaMinima) && parseInt(peticion.temperaturaMinima)){
		const min = parseInt(peticion.temperaturaMinima);
		const max = parseInt(peticion.temperaturaMinima);
		mySerial.write(`TEMPERATURA: ${min} ${max}`);
	}
	if(parseInt(peticion.humedadMinima) && parseInt(peticion.humedadMinima)){
		const min = parseInt(peticion.humedadMinima);
		const max = parseInt(peticion.humedadMinima);
		mySerial.write(`HUMEDAD: ${min} ${max}`);
	}
	res.status(200).end();
});

app.all('*', function (req, res) {
	res.status(404).end();
});

const Readline = SerialPort.parsers.Readline;
const parser = new Readline('\n');

const mySerial = new SerialPort("/dev/cu.usbmodem1421",{
	baudRate: 9600
});

mySerial.pipe(parser);

mySerial.on('open', ()=>{
	console.log("Puerto Serial Abierto");
	parser.on('data', data =>{
		//console.log(data.toString());
		io.emit('arduino:data',{
			value: data.toString()
		});
	});
	mySerial.on('error', err =>{
		console.log(err.message);
	});
});

server.listen(3000,()=>{
	console.log("Escuchando en puerto ", 3000);
});