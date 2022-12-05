const express = require('express'); 
const app = express();
const sqlite3 = require('sqlite3').verbose();
const router = express.Router();
const bodyParser = require('body-parser');

// Functions
const functions = require('../functions/crud');

const DBPATH = 'dbUser.db';

app.use(express.json()); 
app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

router.get('/', (req, res) => {

  var db = new sqlite3.Database(DBPATH);

  	db.all(functions.readNode(query_data['table'], '*'), [],  (err, areas ) => {
		if (err) {
		    throw err;
		}
    	res.render('pages/edit', {areas: areas});
	});

	db.close();

});

var query_data = {
  table: '`areas`',
  insert_columns: '`name`'
};

router.get('/areas', (req, res) => {

  var db = new sqlite3.Database(DBPATH);

  db.all(functions.readNode(query_data['table'], '*'), [],  (err, areas ) => {
		if (err) {
		    throw err;
		}
    res.json(areas), {areas: areas};
	});
  
	db.close();

});


module.exports = router;