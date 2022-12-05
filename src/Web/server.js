const express = require('express'); 
const app = express();
const mysql = require('mysql');
const bodyParser = require('body-parser');

const DBPATH = 'dbUser.db';

app.use(express.json()); 
app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

// Routes
const userCred = require('./tools/routes/userCred.js');
const editAreas = require('./tools/routes/editAreas.js');
const configSteps = require('./tools/routes/configSteps.js');
const beaconReq = require('./tools/routes/beaconReq.js');


const port = 3000;
app.listen(port, (req, res) => {
    console.log(`Server is running on port ${port}`);
});

// Login page
app.use('/', userCred);
app.use('/users', userCred);
app.use('/users/register', userCred);
app.use('/users/auth', userCred);

// Edit page
app.use('/edit', editAreas);

// Steps page 
app.use('/config', configSteps);

// Beacon 
app.use('/beacon', beaconReq);