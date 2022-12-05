const express = require('express'); 
const app = express();
const sqlite3 = require('sqlite3').verbose();
const router = express.Router();
const bodyParser = require('body-parser');

// Functions
const functions = require('../functions/crud');
const { render } = require('ejs');

var query_data = {
	table: '`beacons`',
	insert_columns: '`Reg_Area`',
};

const DBPATH = 'dbUser.db';


app.use(express.json()); 
app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

router.post('/', (req, res) => {
    res.header("Access-Control-Allow-Origin", "*");
    console.log(req);
    console.log(req.body);
    texto = req.body;
    console.log(texto);
    console.log("Recebi um dado");
    res.send(texto);
  
});

router.get('/', (req, res) => {
res.header("Access-Control-Allow-Origin", "*");
  console.log("Recebi a requisição de dados");
  dados = {
    action: 1,
    sensor: "LED",
    status: "OFF",
  };
  json = JSON.stringify(dados);
  res.send(json);
});

module.exports = router;