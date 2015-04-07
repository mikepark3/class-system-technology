/**
 * New node file
 */
var  http = require('http');
var express = require('express');
var sql = require('mysql');
var app= express();

var connection =sql.createConnection({
   host :'localhost',
   port : 3306,
   user : 'root',
   password : '1234',
   database : 'my_report'
});
connection.connect(function(err){
   if(err){
      console.error('mysql connection error');
      console.error(err);
      throw err;
   }
});
   connection.query('SELECT * FROM my_report.l1_cache_analysis',function(err,rows){
   if(err){
      console.log("mysql failure");
      throw err;
   }
   app.all('/',function(request,response){
      response.writeHead(200,{"Content-Type": "text/html"});
      response.write("<h1>Read me !!\n</h1>");
      response.write("<h1>http://203.246.112.200:10064/hit -> hit rate graph\n</h1>");
      response.write("<h1>http://203.246.112.200:10064/miss -> miss rate graph\n</h1>");
      response.write("<h1>http://203.246.112.200:10064/reuse -> reuse  graph\n</h1>");
      //response.write("http://203.246.112.200:10064/hit -> hit rate graph\n");
      
   });
   
   
   app.all('/hit',function(request,response){
      var data = '[';
      rows.forEach(function(value, index, thisArr){
          data += ('[' + value.Index + ', ');
          data += (+value.HitRate + ' ],');
         // data += (+value.MissRate+'], ');
          //data += (+value.ReuseLine+', ');
         // data += (+value.ReuseLine + '], ');

        });
      //data = data.slice(0, -2);
        data += ']';
       response.writeHead(200,{"Content-Type": "text/html"});
       response.write("<html>\n");
       response.write("<head>\n");
       response.write('<script type="text/javascript" src="https://www.google.com/jsapi"></script>\n');
       response.write('<script type="text/javascript">\n');
       response.write("google.load('visualization', '1.0', {'packages':['corechart']});\n");
       response.write("google.setOnLoadCallback(drawChart);\n");
       response.write("function drawChart() {\n");
       response.write("var data = new google.visualization.DataTable();\n");
       response.write("data.addColumn('number', 'Index');\n");
       response.write("data.addColumn('number', 'HitRate');\n");
      // response.write("data.addColumn('number', 'MissRate');\n");
       //response.write("data.addColumn('number', 'ReuseLine');\n");
      // response.write("data.addColumn('number', 'Instruction');\n");
       console.log(data);
       response.write("data.addRows(" + data + ");\n");
       response.write("var options = {'title':'Using PAPI','width':1028,'height':960};\n");
       response.write("var chart = new google.visualization.ComboChart(document.getElementById('chart_div'));\n");
       response.write(" chart.draw(data, options);\n");
       response.write("}\n");
       response.write("</script>\n");
       response.write("</head>\n");
       response.write("<body>\n");
       response.write("<div id='chart_div'></div>\n");
       response.write("</body>\n");
       response.write("</html>\n");
       //response.redirect
       response.end();
   });
   app.all('/miss',function (request,response){
      var data = '[';
      rows.forEach(function(value, index, thisArr){
          data += ('[' + value.Index + ', ');
          //data += (+value.HitRate + ' ,');
          data += (+value.MissRate+'], ');
          //data += (+value.ReuseLine+', ');
         // data += (+value.ReuseLine + '], ');

        });
      //data = data.slice(0, -2);
        data += ']';
      response.writeHead(200,{"Content-Type": "text/html"});
       response.write("<html>\n");
       response.write("<head>\n");
       response.write('<script type="text/javascript" src="https://www.google.com/jsapi"></script>\n');
       response.write('<script type="text/javascript">\n');
       response.write("google.load('visualization', '1.0', {'packages':['corechart']});\n");
       response.write("google.setOnLoadCallback(drawChart);\n");
       response.write("function drawChart() {\n");
       response.write("var data = new google.visualization.DataTable();\n");
       response.write("data.addColumn('number', 'Index');\n");
       //response.write("data.addColumn('number', 'HitRate');\n");
       response.write("data.addColumn('number', 'MissRate');\n");
       //response.write("data.addColumn('number', 'ReuseLine');\n");
      // response.write("data.addColumn('number', 'Instruction');\n");
       console.log(data);
       response.write("data.addRows(" + data + ");\n");
       response.write("var options = {'title':'Using PAPI','width':1028,'height':960};\n");
       response.write("var chart = new google.visualization.ComboChart(document.getElementById('chart_div'));\n");
       response.write(" chart.draw(data, options);\n");
       response.write("}\n");
       response.write("</script>\n");
       response.write("</head>\n");
       response.write("<body>\n");
       response.write("<div id='chart_div'></div>\n");
       response.write("</body>\n");
       response.write("</html>\n");
       //response.redirect
       response.end();
   });
   app.all('/reuse',function (request,response){
      var data = '[';
      rows.forEach(function(value, index, thisArr){
          data += ('[' + value.Index + ', ');
          //data += (+value.HitRate + ' ,');
          //data += (+value.MissRate+'], ');
          //data += (+value.ReuseLine+', ');
          data += (+value.ReuseLine + '], ');

        });
      //data = data.slice(0, -2);
        data += ']';
      response.writeHead(200,{"Content-Type": "text/html"});
       response.write("<html>\n");
       response.write("<head>\n");
       response.write('<script type="text/javascript" src="https://www.google.com/jsapi"></script>\n');
       response.write('<script type="text/javascript">\n');
       response.write("google.load('visualization', '1.0', {'packages':['corechart']});\n");
       response.write("google.setOnLoadCallback(drawChart);\n");
       response.write("function drawChart() {\n");
       response.write("var data = new google.visualization.DataTable();\n");
       response.write("data.addColumn('number', 'Index');\n");
       //response.write("data.addColumn('number', 'HitRate');\n");
       //response.write("data.addColumn('number', 'MissRate');\n");
       response.write("data.addColumn('number', 'ReuseLine');\n");
      // response.write("data.addColumn('number', 'Instruction');\n");
       console.log(data);
       response.write("data.addRows(" + data + ");\n");
       response.write("var options = {'title':'Using PAPI','width':1028,'height':960};\n");
       response.write("var chart = new google.visualization.ComboChart(document.getElementById('chart_div'));\n");
       response.write(" chart.draw(data, options);\n");
       response.write("}\n");
       response.write("</script>\n");
       response.write("</head>\n");
       response.write("<body>\n");
       response.write("<div id='chart_div'></div>\n");
       response.write("</body>\n");
       response.write("</html>\n");
       //response.redirect
       response.end();
   });
   
   app.all('/hitmiss',function (request,response){
      var data = '[';
      rows.forEach(function(value, index, thisArr){
          data += ('[' + value.Index + ', ');
          data += (+value.HitRate + ' ,');
          data += (+value.MissRate+'], ');
          //data += (+value.ReuseLine+', ');
          //data += (+value.ReuseLine + '], ');

        });
      //data = data.slice(0, -2);
        data += ']';
      response.writeHead(200,{"Content-Type": "text/html"});
       response.write("<html>\n");
       response.write("<head>\n");
       response.write('<script type="text/javascript" src="https://www.google.com/jsapi"></script>\n');
       response.write('<script type="text/javascript">\n');
       response.write("google.load('visualization', '1.0', {'packages':['corechart']});\n");
       response.write("google.setOnLoadCallback(drawChart);\n");
       response.write("function drawChart() {\n");
       response.write("var data = new google.visualization.DataTable();\n");
       response.write("data.addColumn('number', 'Index');\n");
       response.write("data.addColumn('number', 'HitRate');\n");
       response.write("data.addColumn('number', 'MissRate');\n");
       //response.write("data.addColumn('number', 'ReuseLine');\n");
      // response.write("data.addColumn('number', 'Instruction');\n");
       console.log(data);
       response.write("data.addRows(" + data + ");\n");
       response.write("var options = {'title':'Using PAPI','width':1028,'height':960};\n");
       response.write("var chart = new google.visualization.ComboChart(document.getElementById('chart_div'));\n");
       response.write(" chart.draw(data, options);\n");
       response.write("}\n");
       response.write("</script>\n");
       response.write("</head>\n");
       response.write("<body>\n");
       response.write("<div id='chart_div'></div>\n");
       response.write("</body>\n");
       response.write("</html>\n");
       //response.redirect
       response.end();
   });
   
   
   http.createServer(app).listen(8108,function(){
      console.log('Server Running at http://127.0.0.1:8108');
   });
   
}); 