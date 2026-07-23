#ifndef WEB_INDEX_H
#define WEB_INDEX_H

static const char web_index_html[] =
"<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\">"
"<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
"<title>ADXL345</title><style>"
"body{font-family:sans-serif;margin:24px;background:#f7f7f7;color:#111}"
"b{display:inline-block;width:2em}.v{font-size:2em;margin:.3em 0}"
"</style></head><body><h1>ADXL345</h1>"
"<div class=\"v\"><b>X</b><span id=\"x\">--</span></div>"
"<div class=\"v\"><b>Y</b><span id=\"y\">--</span></div>"
"<div class=\"v\"><b>Z</b><span id=\"z\">--</span></div>"
"<p id=\"s\">waiting</p><script>"
"async function t(){try{let r=await fetch('/sensor.json');let j=await r.json();"
"x.textContent=j.x+' g';y.textContent=j.y+' g';z.textContent=j.z+' g';s.textContent='sample '+j.seq}"
"catch(e){s.textContent='offline'}}setInterval(t,300);t()</script></body></html>";

#endif
