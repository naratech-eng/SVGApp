'use strict'

// C library API
const ffi = require('ffi-napi');
var ref = require('ref-napi');
const StructType = require('ref-struct-napi');
const ArrayType = require('ref-array-napi');

// Express App (Routes)
const express = require("express");
const app = express();
const path = require("path");
const fileUpload = require('express-fileupload');
const bodyParser = require('body-parser');

var CharArray256 = ArrayType('char', 256);
var CharArray50 = ArrayType('char', 50);

const _struct_rectangle = StructType({
  x: 'float',
  y: 'float',
  width: 'float',
  height: 'float',
  units: CharArray50,
});

const _struct_circle = StructType({
  cx: 'float',
  cy: 'float',
  r: 'float',
  units: CharArray50,
});

const _struct_path = StructType({
  data: 'char *',
});


const _struct_svg = StructType({
  namespace: CharArray256,
  title: CharArray256,
  description: CharArray256,
  /* TODO: add lists of rectangles, circles, ... here */
});

var rect = ref.refType(_struct_rectangle);
var svg = ref.refType(_struct_svg);


var libsvgparse = ffi.Library("./libsvgparse",
  {
    'JSONtoRect': [rect, ['string']],
    'createSVGimage': [svg, ['string']],
    'writeSVGimage': ['bool', [svg, 'string']],
    'JSONtoSVG': [svg, ['string']],
    'num_rectangles_in_image': ['int', ['string']],
    'num_circles_in_image': ['int', ['string']],
    'num_paths_in_image': ['int', ['string']],
    'num_groups_in_image': ['int', ['string']],
    'SVGtoJSONext': ['string', ['string']],
    'setSVGTitle': ['void', ['string', 'string']],
    'setSVGDescription': ['void', ['string', 'string']],
    'validateSVGimageWithPath': ['bool', ['string', 'string']],
    'addRectangle': ['void', ['string', 'string']],
    'addCircle': ['void', ['string', 'string']]
  }
);

var schemafile = 'svg.xsd';

app.use(fileUpload());
app.use(express.static(path.join(__dirname + '/uploads')));
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.json());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/', function (req, res) {
  res.sendFile(path.join(__dirname + '/public/index.html'));
});

// Send Style, do not change
app.get('/style.css', function (req, res) {
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname + '/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js', function (req, res) {
  fs.readFile(path.join(__dirname + '/public/index.js'), 'utf8', function (err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, { compact: true, controlFlowFlattening: true });
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/uploads', function (req, res) {
  console.log("received request: " + req);
  console.log("req.files: " + req.files)
  console.log("req.file: " + req.files.uploadFileName);
  if (!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFileName;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function (err) {
    if (err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

app.post('/createsvg', function (req, res) {
  console.log("received create svg request: " + req);
  console.log("image name: " + req.body.svgname);

  var img = libsvgparse.JSONtoSVG("{\"title\":\"\",\"descr\":\"\"}");

  var namespace = Buffer.alloc(256);
  namespace.write("http://www.w3.org/2000/svg");

  img.deref().namespace = new CharArray256(namespace);
  console.log("created image: " + img.deref());
  console.log("namespace in img: " + img.deref().namespace);
  libsvgparse.writeSVGimage(img, 'uploads/' + req.body.svgname + '.svg');

  res.redirect('/');
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function (req, res) {
  fs.stat('uploads/' + req.params.name, function (err, stat) {
    if (err == null) {
      res.sendFile(path.join(__dirname + '/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: ' + err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
// app.get('/someendpoint', function (req, res) {
//   let retStr = req.query.name1 + " " + req.query.name2;
//   res.send({
//     foo: retStr
//   });
// });

app.get('/returnInt', function (req, res) {
  let ret = libsvgparse.returnInt();
  res.send({
    foo: ret
  });
});

app.get('/returnStr', function (req, res) {
  let ret = libsvgparse.returnString();
  res.send({
    foo: ret
  });
});

app.get('/modifyStr', function (req, res) {
  let ret = libsvgparse.modifyString("test");
  res.send({
    foo: ret
  });
});

app.get('/countRectangles', function (req, res) {
  let ret = libsvgparse.num_rectangles_in_image("uploads/" + req.query.file);
  console.log("request: " + req.params);
  console.log("request.file: " + req.query.file);
  console.log("created obj: " + ret);
  res.send({
    ret: ret
  });
});

app.get('/jsontorect', function (req, res) {
  let ret = libsvgparse.JSONtoRect("{\"x\":500,\"y\":330,\"w\":10,\"h\":4,\"units\":\"cm\"}");
  res.send({
    foo: ret
  });
  console.log(ret);
  console.log(ret.deref().x);
});

app.get('/listsvgfiles', function (req, res) {
  let filelist = [];

  fs.readdir('uploads/', (err, files) => {

    console.log("found files: " + files);

    files.forEach(function (file) {
      if ((file !== '.') && (libsvgparse.validateSVGimageWithPath('uploads/' + file, schemafile) == true)) {
        let rects = libsvgparse.num_rectangles_in_image('uploads/' + file);
        let circles = libsvgparse.num_circles_in_image('uploads/' + file);
        let paths = libsvgparse.num_paths_in_image('uploads/' + file);
        let groups = libsvgparse.num_groups_in_image('uploads/' + file);

        let stats = fs.statSync('uploads/' + file);
        let sizeInkB = Math.round(stats["size"] / 1024);

        let f = {
          file: file,
          rects: rects,
          circles: circles,
          paths: paths,
          groups: groups,
          size: sizeInkB
        };

        filelist.push(f);


      } else {
        console.log("Ignored invalid file: " + file);
      }
    });


    res.send({
      filelist
    });

  });
});

app.get('/showsvgfile', function (req, res) {
  let ret;
  if ((req.query.file !== undefined) && (req.query.file !== '')) {
    console.log("requested information on file: " + req.query.file);
    let str = libsvgparse.SVGtoJSONext("uploads/" + req.query.file);

    ret = JSON.parse(str);
  }
  res.send({
    ret: ret
  });
});

app.put('/updatetitle', function (req, res) {
  console.log("updating title for file: " + req.body.file + " to " + req.body.title);

  libsvgparse.setSVGTitle('uploads/' + req.body.file, req.body.title);
});

app.post('/addRectRoute', function (req, res) {
  console.log("adding rectangle to image " + req.body.viewSelectAddRect);

  var rect = {
    x: parseFloat(req.body.rectX),
    y: parseFloat(req.body.rectY),
    w: parseFloat(req.body.rectW),
    h: parseFloat(req.body.rectH),
    fill: req.body.rectFill,
    stroke: req.body.rectSTR,
    units: "cm"
  };

  libsvgparse.addRectangle('uploads/' + req.body.viewSelectAddRect, JSON.stringify(rect));

  res.redirect('/');
});

app.post('/addCircleRoute', function (req, res) {
  console.log("adding circle to image " + req.body.viewSelectAddCircle);

  var circ = {
    cx: parseFloat(req.body.circCX),
    cy: parseFloat(req.body.circCY),
    r: parseFloat(req.body.circR),
    fill: req.body.circFill,
    stroke: req.body.circSTR,
    units: "cm"
  };

  libsvgparse.addCircle('uploads/' + req.body.viewSelectAddCircle, JSON.stringify(circ));

  res.redirect('/');
});

app.put('/updatedescription', function (req, res) {
  console.log("updating desc for file: " + req.body.file + " to " + req.body.desc);

  libsvgparse.setSVGDescription('uploads/' + req.body.file, req.body.desc);
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
