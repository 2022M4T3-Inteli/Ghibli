const express = require('express'); 
const app = express();
const sqlite3 = require('sqlite3').verbose();
const router = express.Router();
const bodyParser = require('body-parser');

let areaID = 0;
let date = 0; 

const DBPATH = 'dbUser.db';

// Functions
const functions = require('../functions/crud');

app.use(express.json()); 
app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

var query_data = {
    table: '`areas`',
    table2: '`history`',
    table3: '`tags`', 
    table4: '`users`'
};

router.get('/:id/:dates', (req, res) => {
  var db = new sqlite3.Database(DBPATH);

    db.all(functions.readNode(query_data['table'], '*'), (err, areas ) => {
		if (err) {
		    throw err;
		}
        db.all(functions.readNode(query_data['table2'], '*'),  (err, history ) => {
            if (err) {
                throw err;
            }
            db.all(functions.readNode(query_data['table3'], '*'),  (err, tags) => {
                if (err) {
                    throw err;
                }
                db.all(functions.readNode(query_data['table4'], '*'),  (err, users ) => {
                    if (err) {
                        throw err;
                    }
                    res.render('pages/dashboard', {area_ID: req.params.id, date: req.params.dates, users: users, areas: areas, history: history, tags: tags});
                });
            });
        });
    });
 db.close();

});

router.post('/areas', (req, res) => {
    areaID = req.body.areaID;
    res.redirect('/dashboard/' + areaID + '/' + date);
});

router.post('/dates', (req, res) => {
    date = req.body.date;
    console.log(date);
    res.redirect('/dashboard/' + areaID + '/' + date);
});

module.exports = router;