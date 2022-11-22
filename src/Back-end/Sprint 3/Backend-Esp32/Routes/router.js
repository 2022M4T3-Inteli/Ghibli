const express = require('express');
const router = express.Router();

router.get('/name', (req, res) => {  
    res.send('name');
});

router.get('/admin', (req, res) => {  
    res.send('admin');
});

router.get('/id', (req, res) => {  
    res.send('id');
});

module.exports = router;