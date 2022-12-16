const express = require("express");
const app = express();
const sqlite3 = require("sqlite3").verbose();
const router = express.Router();
const bodyParser = require("body-parser");

let MACAddress = [];
let value;

// Action 0 é registro Inicial
// Action 1 é esperar por instruções

let dados = {

};

const DBPATH = "dbUser.db";

// Functions
const functions = require("../functions/crud");
const { text } = require("body-parser");

var query_data = {
  table: "`beacons`",
  table2: "`tags`",
  table3: "`users`",
  table4: "`areas`",
  table5: "`history`",
  create_columns: "`Reg_Area`, `Name`, `Mac_Add`, `password`",
  create_columns2: "`Name`, `MacAddress`, `connected`, `area_id`",
  create_columns3: "`date`, `area_id`, `time`, `userid`, `state`",
  rfid_columns: "`RFID`"
};

app.use(express.json());
app.use(express.static("public"));
app.use(bodyParser.urlencoded({ extended: true }));
app.set("view engine", "ejs");

router.post("/tags", (req, res) => {

  texto = req.body;

  console.log(texto);

  console.log("Recebi um dado");

  res.send(texto);

  var db = new sqlite3.Database(DBPATH);

  db.all(functions.readNode(query_data["table2"], "*"), [], async (err, tags) => {

      if (err) {
        throw err;
      }

      for (let i = 0; i < tags.length; i++) {
        if (MACAddress.includes(texto.MACAddress)) {
        } else {
          MACAddress.push(tags[i].MacAddress);
        }
      }

    if (MACAddress.includes(texto.MACAddress)) {

        console.log("MACAddress já existe");

        if(texto.RFID) { 
            db.all(
                functions.readNode(query_data["table3"], "*", "register = 1"), [], async (err, users) => {
                    if (err) {
                    throw err;
                    }
                    if (users.length != 0) {
                        for (let i = 0; i < users.length; i++) {
                        if (users[i].RFID == 0) {
                            db.run(functions.updateNode(query_data["table3"], query_data["rfid_columns"], texto.RFID,"ID = " + users[i].ID), [], async (err, user) => {
                                    if (err) {
                                        throw err;
                                    }
                                    console.log("RFID adicionado");
                                    console.log(user);
                                    i = users.length;
                                });
                        }
                        }
                    } else {
                        db.all(functions.readNode(query_data["table3"], "*", "RFID = " + texto.RFID), [], async (err, users) => {
                            if (err) {
                                throw err;
                            }
                            console.log(users[0].funcid);
                            if (users.length != 0) {
                                db.all(functions.readNode(query_data["table2"], "*", "MacAddress = '" + texto.MACAddress + "'"), [], async(err, tag) => {
                                    if (err) {
                                        throw err;
                                    }
                                    if (tag[0].connected == 0) {
                                        value = users[0].funcid;
                                    } else {
                                        value = 0;
                                    }
                                    db.run(functions.updateNode(query_data["table2"], '`connected`', value, "MacAddress = '" + texto.MACAddress + "'"), [], async (err, user) => {
                                        if (err) {
                                            throw err;
                                        }
                                        if (value != 0) {
                                            console.log("Usuário Conectado");
                                        } else {
                                            console.log("Usuário Desconectado");
                                        }
                                    });
                                });
                            }
                        });
                    }
            });
        } else if (texto.position && texto.area_ID != 0) {

            db.all(functions.readNode(query_data["table5"], "*", "userid = '" + texto.user_ID + "'"), [], async (err, history) => {
                if (err) {
                    throw err;
                }
                let latestValue = history[history.length - 1].state;
                if (latestValue == texto.position) {
                    console.log("Posição já registrada");
                } else {

                    let date = new Date();
                    let year = date.getFullYear();
                    let month = date.getMonth();
                    let day = date.getDay();
                    let time = date.toLocaleTimeString();
                    if (day < 10) {
                        day = "0" + day;
                    }
                    let current_date = `${day} / ${month} / ${year}`;
        
                    console.log(current_date);
                    console.log(time);
        
                    db.all(functions.createNode(query_data["table5"], query_data["create_columns3"], "'" + current_date + "', '" + texto.area_ID + "', '" + time + "', '" + texto.user_ID + "', '" + texto.position + "'"), [], async (err, user) => {
                        if (err) {
                            throw err;
                        }
                        if (texto.position == "In") {
                            console.log("Entrada Registrada");
                        } else {
                            console.log("Saída Registrada");
                        }
                    });
                }
            });
        };
        dados[texto.MACAddress] = 0;

    } else {
        console.log("MACAddress não existe");
        db.run(
          functions.createNode( query_data["table2"], query_data["create_columns2"], "'" + texto.name + "', '" + texto.MACAddress + "', '" + 0 +  "', '" + 0 + "'") , [], async (err, beacons) => {
            if (err) {
              throw err;
            }
            console.log("MACAddress adicionado");
            dados[texto.MACAddress] = 0;
        });
    }
  });

  db.close();
});

router.post("/beacon", (req, res) => {

  texto = req.body;
  console.log(texto);
  console.log("Recebi um dado");
  res.send(texto);

  var db = new sqlite3.Database(DBPATH);

  db.all(
    functions.readNode(query_data["table"], "*"),
    [],
    async (err, beacons) => {
      if (err) {
        throw err;
      }

      for (let i = 0; i < beacons.length; i++) {
        if (MACAddress.includes(texto.MACAddress)) {
        } else {
          MACAddress.push(beacons[i].Mac_Add);
        }
      }

      if (MACAddress.includes(texto.MACAddress)) {
        console.log("MACAddress já existe");
        dados[texto.MACAddress] = 1;
      } else {
        console.log("MACAddress não existe");
        db.run(
          functions.createNode(query_data["table"], query_data["create_columns"],
            0 + ", '" + texto.name + "', '" + texto.MACAddress +  + "', '" + texto.password + "'"
          )
        ),
          [],
          async (err, beacons) => {
            if (err) {
              throw err;
            }
            console.log("MACAddress adicionado");
            dados[texto.MACAddress] = 1;
          };
      }
    }
  );

  db.close();
});

router.get("/beacon", (req, res) => {

  res.header("Access-Control-Allow-Origin", "*");
  console.log("Recebi a requisição de dados");

  var db = new sqlite3.Database(DBPATH);

    db.all(functions.readNode(query_data["table4"], "*"), [], async (err, area) => {
        if (err) {
            throw err;
        }
        for (let i = 0; i < area.length; i++) {
            dados[area[i].ID] = {
                name: area[i].Name,
                width: area[i].Width, 
                length: area[i].Length,
                SSIDS: [],
                PWDS: []
            };
        }

        db.all(functions.readNode(query_data["table"], "*", "Reg_Area != 0"), [], async (err, beacons) => {

            console.log(beacons);

            if (err) {
                throw err;
            }
            for (let i = 0; i < beacons.length; i++) {
                dados[beacons[i].Reg_Area]['SSIDS'].push(beacons[i].Name)
                dados[beacons[i].Reg_Area]['PWDS'].push(beacons[i].password)
            }
        });
    });

    console.log(dados);
    json = JSON.stringify(dados);
    res.send(json);
});

router.get("/tag", (req, res) => {
    res.header("Access-Control-Allow-Origin", "*");
    console.log("Recebi a requisição de dados");
    console.log(dados);
    json = JSON.stringify(dados);
    res.send(json);
});

module.exports = router;
