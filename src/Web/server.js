require('dotenv').config();

const express = require('express'); 
const app = express();
const bodyParser = require('body-parser');

app.use(express.json()); 
app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

// Routes
const userCred = require('./tools/routes/userCred.js');
const editAreas = require('./tools/routes/editAreas.js');
const configSteps = require('./tools/routes/configSteps.js');
const beaconReq = require('./tools/routes/beaconReq.js');
const dashboard = require('./tools/routes/dashboard.js');
const tagReq = require('./tools/routes/tagReq.js');
const regESP = require('./tools/routes/registerESP.js');


const port = 3000;
app.listen(port, (req, res) => {
    console.log(`Server is running on port ${port}`);
});

app.get('/token', (req, res) => {
    const refreshToken = req.body.token;
    if (refreshToken == null) return res.sendStatus(401);
    if (!refreshTokens.includes(refreshToken)) return res.sendStatus(403);
    jwt.verify(refreshToken, process.env.REFRESH_TOKEN_SECRET, (err, user) => {
        if (err) return res.sendStatus(403);
        const acessToken = generateAccessToken({ name: user.name });
        res.json({ acessToken: acessToken });
    });
});

// Login page
app.use('/', userCred);
app.use('/users', userCred);
app.use('/users/register', userCred);
app.use('/users/auth', userCred);
app.use('/users/token', userCred);

// Edit page
app.use('/edit', editAreas);

// Steps page 
app.use('/config', configSteps);

// Beacon 
app.use('/beacon', beaconReq);

// Dashboard 
app.use('/dashboard', dashboard);

// Tags Page 
app.use('/tags', tagReq);

// Register ESP
app.use('/registerESP', regESP);