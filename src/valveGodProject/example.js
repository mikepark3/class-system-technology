/*
//simple nodejs server example
var http = require('http');
http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('Hello World\n');
}).listen(8080, '192.168.0.2');
console.log('Server running at http://192.168.0.2:8080/');
*/


//google visualization
var http= require('http');
var server = http.createServer(function (req, res){
  res.writeHead(200,{"Content-Type": "text/html"});
  res.write("<html>\n");
  res.write("<head>\n");
  res.write('<script type="text/javascript" src="https://www.google.com/jsapi"></script>\n');
  res.write('<script type="text/javascript">\n');
  res.write("google.load('visualization', '1.0', {'packages':['corechart']});\n");
  res.write("google.setOnLoadCallback(drawChart);\n");
  res.write("function drawChart() {\n");
  res.write("var data = new google.visualization.DataTable();\n");
  res.write("data.addColumn('string', 'Topping');\n");
  res.write("data.addColumn('number', 'Slices');\n");
  res.write("data.addRows([['Mushrooms', 3],['Onions', 1],['Olives', 1],['Zucchini', 1],['Pepperoni', 2]]);\n");
  res.write("var options = {'title':'How Much Pizza I Ate Last Night','width':400,'height':300};\n");
  res.write("var chart = new google.visualization.BarChart(document.getElementById('chart_div'));\n");
  res.write("chart.draw(data, options);\n");
  res.write("}\n");
  res.write("</script>\n");
  res.write("</head>\n");
  res.write("<body>\n");
  res.write('<div id="chart_div" style="width:400; height:300"></div>\n');
  res.write("</body>\n");
  res.write("</html>\n");
  res.end("Hello World\n");
});
server.listen(8080, '192.168.0.2');
console.log('Server running at http://192.168.0.2:8080/');


//mysql
var mysql = require('mysql');
var connection = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '20093291',
  database: 'project1'
});
connection.connect(function(err){
  if(err){ 
    console.log('connect error');
    throw err;
  }
});
connection.query('insert into testData(data, temp) values(?, ?)', [new Date(), 14.01], function(err){
  if(err){
    console.log('insert query error');
    throw err;
  }
});
connection.query('select * from testData', function(err,rows,cols){
  if(err){
    console.log('select query error');
    throw err;
  }
  console.log(rows);
});
connection.end();
