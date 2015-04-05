//mysql
var mysql = require('mysql');
var connection = mysql.createConnection({
  host: '192.168.0.5',
  user: 'root',
  password: '1234',
  database: 'my_report'
});
connection.connect(function(err){
  if(err){ 
    console.log('connect error');
    throw err;
  }
});
connection.query('select * from mydata', function(err,rows,cols){
  var data = '[';
  if(err){
    console.log('select query error');
    throw err;
  }
  rows.every(function(value, index, thisArr){
    //if(index != 0)
      data += ('[' + index + ', ');
      data += (value.temp + ', ');
      data += (value.time + '], ');
    //}
    if(index == 100)
      return false;
    else
      return true;
  });
  /*
  rows.forEach(function(value, index, thisArr){
    data += ('[' + index + ', ');
    data += (value.celsius_floor + ', ');
    data += (value.celsius_cage + '], ');
    if(index == 10)
      break;
  });
  */
  data = data.slice(0, -2);
  data += ']';
  //console.log(data);

  //google visualization
  var http= require('http');
  var server = http.createServer(function (req, res){
    res.writeHead(200,{"Content-Type": "text/html"});
    res.write("<html>\n");
    res.write("<head>\n");
    res.write('<script type="text/javascript" src="https://www.google.com/jsapi"></script>\n');
    res.write('<script type="text/javascript">\n');
    res.write("google.load('visualization', '1.1', {'packages':['line']});\n");
    res.write("google.setOnLoadCallback(drawChart);\n");
    res.write("function drawChart() {\n");
    res.write("var data = new google.visualization.DataTable();\n");
    res.write("data.addColumn('number', 'Time');\n");
    res.write("data.addColumn('number', 'Floor');\n");
    res.write("data.addColumn('number', 'Cage');\n");

    data = ("data.addRows(" + data + ");\n");
    res.write(data);

    res.write("var options = {'chart': {'title': 'Temperature graph', 'subtitle': 'Save my hedgehog!'}, 'width':1300, 'height':500};\n");
    res.write("var chart = new google.charts.Line(document.getElementById('linechart_material'));\n");
    res.write("chart.draw(data, options);\n");
    res.write("}\n");
    res.write("</script>\n");
    res.write("</head>\n");
    res.write("<body>\n");
    res.write('<div id="linechart_material"></div>\n');
    res.write("</body>\n");
    res.write("</html>\n");
    res.end();
  });
  server.listen(8000, '192.168.0.2');
  console.log('Server running at Internal address http://192.168.0.2:8000/');
  console.log('Server running at External address http://203.246.112.200:10073/');
});
connection.end();


