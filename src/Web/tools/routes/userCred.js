const express = require('express'); 
const app = express();
const bcrypt = require('bcrypt');
const sqlite3 = require('sqlite3').verbose();
const router = express.Router();
const bodyParser = require('body-parser');
const jwt = require('jsonwebtoken');


const DBPATH = 'dbUser.db';

// Functions
const functions = require('../functions/crud');
const Authorization = require('../functions/authorization');

app.use(express.json()); 
app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

router.get('/', (req, res) => {
  res.render('pages/login');
});

var query_data = {
  table: '`login_auth`',
  insert_columns: '`funcid`, `password`',
  insert_columns2: '`funcid`, `password`, `user_creation_token`',
  refresh_update: '`refresh_token`'
};

router.get('/users', (req, res) => {

  var db = new sqlite3.Database(DBPATH);

  db.all(functions.readNode(query_data['table'], '*'), [],  (err, users ) => {
		if (err) {
		    throw err;
		}
		res.json(users);
	});
	db.close();
  
});

router.post('/users/new', (req, res) => {

  var db = new sqlite3.Database(DBPATH);

  console.log(req.body);

  db.all(functions.readNode(query_data['table'], '*'), [], async (err, users ) => {
		if (err) {
		    throw err;
		}
    const user = users.find(user => user.user_creation_token == req.body.user_creation_token)
    if (user == null) {
      return res.status(400).send('Invalid User Creation Token');
    } else {
      return res.status(201).send('Valid Token');
    }
	});

	db.close();

});

router.post('/users/register', async (req,res) => {

  var db = new sqlite3.Database(DBPATH);

  db.all(functions.readNode(query_data['table'], '*'), [], async (err, users ) => {

    console.log(req.body);

		if (err) {
		    throw err;
		}

    try {

      const hashedPassword = await bcrypt.hash(req.body.password, 10);

      db.run(functions.createNode(query_data['table'], query_data['insert_columns2'], "'" + req.body.funcid + "', '" + hashedPassword + "'," + "'" + req.body.user_creation_token + "'"), [], function(err) {
        if (err) {
          return console.error(err.message);
        }
        console.log(`Rows inserted ${this.changes}`);
      });

      res.status(201).send();

    } catch {

      res.status(500).send();

    }
	});

});

router.post('/users/auth', async (req, res) => {

  var db = new sqlite3.Database(DBPATH);

  console.log(req.body);

  db.all(functions.readNode(query_data['table'], '*'), [],  async (err, users ) => {
		if (err) {
		    throw err;
		}
    const user = users.find(user => user.funcid == req.body.funcid)
    if (user == null) {
      return res.status(400).send('Cannot find user');
    }
    try {
      if(await bcrypt.compare(req.body.password, user.password)) {
        // User is authenticated

        console.log("User is authenticated");

        res.redirect('/dashboard/id = 0/date = 0');

      } else {
        res.send('Not Allowed');
      }
    } catch {
      res.status(500).send();
    }
	});
  
	db.close();
});

router.post('/users/token', async (req, res) => {

  const user = req.body.user

  var db = new sqlite3.Database(DBPATH);

  const acessToken = Authorization.generateAccessToken({ name: user});

  const refreshToken = jwt.sign(user, process.env.REFRESH_TOKEN_SECRET);

  let hashedRefreshToken = await bcrypt.hash(refreshToken, 10);

  db.run(functions.updateNode(query_data['table'], query_data['refresh_update'], hashedRefreshToken, 'funcid =' + "'" + user + "'"), [], async function(err) {
    if (err) {
      return console.error(err);
    }
    console.log(`Rows updated ${this.changes}`);
  });

  db.close();

  res.send({ acessToken: acessToken, refreshToken: refreshToken, user: user });
});


module.exports = router;