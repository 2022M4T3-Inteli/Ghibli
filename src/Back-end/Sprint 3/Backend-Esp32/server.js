const express = require('express');
const bodyParser = require('body-parser');
const Router = require('./Routes/router');

const app  = express(); 

app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

app.use('/host', Router);

app.listen('3000', (req, res) => {
    console.log('Server is running on port 3000');
});