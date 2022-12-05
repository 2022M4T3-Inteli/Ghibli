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
	if (req.body['area_id'] == 0) {
		var current_step = 0; 
	} else {
		var current_step = 4; 
	}
	res.render('pages/steps', {current_step: current_step});
});

router.post('/next', (req, res) => {
	current_step = req.body['current_step'];

	if (current_step == 0) {

		var db = new sqlite3.Database(DBPATH); 

		available_beacons = [];

		db.all(functions.readNode(query_data['table'], '*', query_data['insert_columns'] + '== 0'), [],  (err, available_beacons) => {
			if (err) {
				throw err;
			}
			res.render('pages/steps', {current_step: current_step + 1, info: available_beacons});
		});
	}

});

module.exports = router;