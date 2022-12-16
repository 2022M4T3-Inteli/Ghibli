const express = require('express'); 
const app = express();
const sqlite3 = require('sqlite3').verbose();
const router = express.Router();
const bodyParser = require('body-parser');

let AreaID; 

const DBPATH = 'dbUser.db';

let dados = {};

// Functions
const functions = require('../functions/crud');

app.use(express.json()); 
app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

var query_data = {
    table: '`areas`',
    table2: '`beacons`',
    step1_columns: '`Name`, `Width`, `Length`, `BeaconModel`',
    create_new: 'Name, Width, Length, BeaconModel'
};

router.post('/connect', (req, res) => { // Conectar o beacon na área 
  console.log(AreaID);
  console.log(req.body);

  var db = new sqlite3.Database(DBPATH);

  let value;

  console.log(req.body.action);

  if (req.body.action == "disconnect") {
    value = 0;
  } else {
    value = AreaID;
  }
  
  db.run(functions.updateNode(query_data['table2'], `Reg_Area`, value, "ID = " + req.body.beacon_id), [], function(err) {
    if (err) {
      return console.error(err.message);
    }
    console.log(`Rows inserted ${this.changes}`);
  
  db.close();
  });

  res.redirect("/beacon/step4");

});

router.post('/create-new', (req, res) => {
    
  var db = new sqlite3.Database(DBPATH);

  db.run(functions.createNode(query_data['table'], query_data['create_new'], "0,0,0,0" ), [], function(err) {
    if (err) {
      return console.error(err.message);
    }
    console.log(`Rows inserted ${this.changes}`);
  
  db.close();
  });

  db.all(functions.readNode(query_data['table'], '*'), [],  (err, users ) => {
		if (err) {
		    throw err;
		}
    let new_id = users[users.length - 1].ID;

    AreaID = new_id;

    console.log("/beacon/step2?AreaID="+new_id);

    res.redirect("/beacon/step2?AreaID="+new_id);
	});

});

router.post('/edit-existing', (req, res) => {
  console.log(req.body);
  AreaID = req.body.areaID;
  res.redirect("/beacon/step2?AreaID="+AreaID);
});

router.get('/', (req, res) => {
  
  var db = new sqlite3.Database(DBPATH);

  db.all(functions.readNode(query_data['table'], '*'), [],  (err, areas ) => {
		if (err) {
		    throw err;
		}
		console.log(areas);
    res.render('pages/beaconedit1', {areas: areas});
	});
 db.close();

});

router.get('/step2', (req, res) => {
    
    var db = new sqlite3.Database(DBPATH);
  
    db.all(functions.readNode(query_data['table'], "*", "ID = " + AreaID), [],  (err, area ) => {
          if (err) {
              throw err;
          }
          console.log(area);
          res.render('pages/beaconedit2', {area: area});
      });
   db.close();
  
});

router.post('/update', (req, res) => {

  console.log(req.body);

  var db = new sqlite3.Database(DBPATH);

  db.run(functions.updateNode(query_data['table'], query_data['step1_columns'], req.body.name + "', '" + req.body.comprimento + "', '" + req.body.largura + "', '" + req.body.BeaconModel, "id=" + AreaID), [], (err, beacon) => {
    if (err) {
      return console.error(err);
    }
    console.log(beacon);
    res.redirect('/beacon/step3');
  });
  db.close();
});

router.get('/step3', (req, res) => {

    console.log(req.body);
    
    var db = new sqlite3.Database(DBPATH);
  
    db.all(functions.readNode(query_data['table'], '*'), [],  (err, users ) => {
          if (err) {
              throw err;
          }
          console.log(users);
          res.render('pages/beaconedit3', {users: users});
      });
   db.close();
  
});

router.get('/step4', (req, res) => {
    
    var db = new sqlite3.Database(DBPATH);
  
    db.all(functions.readNode(query_data['table2'], '*'), [],  (err, beacons ) => {
          if (err) {
              throw err;
          }
          console.log(beacons);
          res.render('pages/beaconedit4', {beacons: beacons, area_ID: AreaID});
      });
   db.close();
  
});

router.get('/step5', (req, res) => {
    
  var db = new sqlite3.Database(DBPATH);

  db.all(functions.readNode(query_data['table'], '*'), [],  (err, users ) => {
        if (err) {
            throw err;
        }
        console.log(users);
        res.render('pages/beaconedit5', {users: users});
    });
 db.close();

});


router.post('/submit', (req, res) => {
    var db = new sqlite3.Database(DBPATH);
}); 


module.exports = router;