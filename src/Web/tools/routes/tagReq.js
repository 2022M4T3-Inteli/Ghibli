const express = require('express'); 
const app = express();
const sqlite3 = require('sqlite3').verbose();
const router = express.Router();
const bodyParser = require('body-parser');

let userID;


const DBPATH = 'dbUser.db';

// Functions
const functions = require('../functions/crud');

app.use(express.json()); 
app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

var query_data = {
    table: '`users`',
    table2: '`tags`',
    create_new: '`FirstName`, `LastName`, `funcid`, `RFID`, `register`',
    step1_columns: '`FirstName`, `LastName`, `funcid`', 
    register: '`register`'
};

router.post('/edit-existing', (req, res) => {
    console.log(req.body);
    userID = req.body.userID;
    res.redirect("/tags/step2?userID="+userID);
});
  

router.post('/create-new', (req, res) => {
    
    var db = new sqlite3.Database(DBPATH);
  
    db.run(functions.createNode(query_data['table'], query_data['create_new'], "0,0,0,0,0" ), [], function(err) {
      if (err) {
        return console.error(err.message);
      }
      console.log(`Rows inserted ${this.changes}`);
    });

    db.all(functions.readNode(query_data['table'], '*'), [],  (err, users ) => {
		if (err) {
		    throw err;
		}
    let new_id = users[users.length - 1].ID;

    userID = new_id;

    console.log("/tags/step2?AreaID="+new_id);

    res.redirect("/tags/step2?AreaID="+new_id);
    });

    db.close();
});

router.get('/', (req, res) => {
    
  var db = new sqlite3.Database(DBPATH);

  db.all(functions.readNode(query_data['table'], '*'), [],  (err, users ) => {
		if (err) {
		    throw err;
		}
		console.log(users);
        res.render('pages/tagedit1', {users: users});
	});
 db.close();

});

router.post('/update', (req, res) => {

    console.log(req.body);
  
    var db = new sqlite3.Database(DBPATH);
  
    db.run(functions.updateNode(query_data['table'], query_data['step1_columns'], req.body.first + "', '" + req.body.last + "', '" + req.body.funcid , "id=" + userID), [], (err, user) => {
      if (err) {
        return console.error(err);
      }
      console.log(user);
      res.redirect('/tags/step3');
    });
    db.close();
});

router.get('/step2', (req, res) => {
    
    var db = new sqlite3.Database(DBPATH);
  
    db.all(functions.readNode(query_data['table'], "*", "ID = " + userID), [],  (err, user ) => {
          if (err) {
              throw err;
          }
          console.log(user);
          res.render('pages/tagedit2', {user: user});
      });

   db.close();
  
});

router.get('/step3', (req, res) => {
    
    var db = new sqlite3.Database(DBPATH);
  
    db.all(functions.readNode(query_data['table'], '*'), [],  (err, users ) => {
          if (err) {
              throw err;
          }
          console.log(users);
          res.render('pages/tagedit3', {users: users});
      });
   db.close();
  
});

router.get('/step4', (req, res) => {
    
    var db = new sqlite3.Database(DBPATH);
  
    db.all(functions.readNode(query_data['table'], '*',  "ID = " + userID), [],  (err, user ) => {
          if (err) {
              throw err;
          }
          console.log(user);
          res.render('pages/tagedit4', {user: user});
      });
   db.close();
  
  });

router.post('/submit', (req, res) => {
    var db = new sqlite3.Database(DBPATH);
}); 

router.post('/RFID', (req, res) => {
    var db = new sqlite3.Database(DBPATH);

    db.run(functions.updateNode(query_data['table'], query_data['register'], 1, "id=" + userID), [], (err, user) => {
        if (err) {
            return console.error(err);
        }
        console.log(user);
        res.redirect('/tags/step4'); 
    });

    console.log(req.body);
});


module.exports = router;