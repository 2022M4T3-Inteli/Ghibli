const jwt = require('jsonwebtoken');

function authenticateToken(req, res, next) {
    const authHeader = req.headers['authorization'];
    const token = authHeader && authHeader.split(' ')[1];
    if (token == null) return res.sendStatus(401);
    jwt.verify(token, process.env.ACESS_TOKEN_SECRET, (err, user) => {
        console.log(err);
        if (err) return res.sendStatus(403);
        req.user = user;
        next();
    })
}

function generateAccessToken(user) {
    return jwt.sign(user, process.env.ACESS_TOKEN_SECRET, { expiresIn: '15s' });
}

module.exports = {generateAccessToken, authenticateToken};