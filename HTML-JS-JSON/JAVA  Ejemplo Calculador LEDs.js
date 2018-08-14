<html><head><title>Calculadora de LEDs</title>
<script language="javascript">
function calculaLeds(){
var valCantLED = parseFloat(document.getElementById("CantLED").value,10);
var valTenLED = parseFloat(document.getElementById("TenLED").value,10);
var valCorLED = parseFloat(document.getElementById("CorLED").value,10);
var valTenRED = parseFloat(document.getElementById("TenRED").value,10);
var valFrecRED = parseFloat(document.getElementById("FrecRED").value,10);
var valResist = parseFloat(document.getElementById("Resist").value,10);
var valCapacit = document.getElementById("Capacit");
var valCapacitAux = (1/(2*Math.PI*valFrecRED*Math.sqrt(Math.pow((valTenRED-(valTenLED*valCantLED))/valCorLED,2)-Math.pow(valResist,2))))*1000000;
valCapacit.value = valCapacitAux.toFixed(4);
} 
</script>
</head>
<body>
<table border="1" cellpadding="2" cellspacing="2" style="text-align: left; width: 384px;">
<tbody>
<tr>
<td style="width: 223px;">Cantidad de LEDs (en serie)</td>
<td style="width: 68px;"><input align="right" id="CantLED" name="CantLED" size="7" style="text-align: right;" /></td>
<td style="width: 65px;"></td>
</tr>
<tr>
<td style="width: 223px;">Tensión de LED &nbsp;
&nbsp;</td>
<td style="width: 68px;"><input align="right" id="TenLED" name="TenLED" size="7" style="text-align: right;" /></td>
<td style="width: 65px;">[V]</td>
</tr>
<tr>
<td style="width: 223px;">Corriente del LED</td>
<td style="width: 68px;"><input align="right" id="CorLED" name="CorLED" size="7" style="text-align: right;" /></td>
<td style="width: 65px;">[A]</td>
</tr>
<tr>
<td style="width: 223px;">Tensión de red </td>
<td style="width: 68px;"><input align="right" id="TenRED" name="TenRED" size="7" style="text-align: right;" value="220" /></td>
<td style="width: 65px;">[V]</td>
</tr>
<tr>
<td style="width: 223px;">Frecuencia de red </td>
<td style="width: 68px;"><input align="right" id="FrecRED" name="FrecRED" size="7" style="text-align: right;" value="50" /></td>
<td style="width: 65px;">[Hz]</td>
</tr>
<tr>
<td style="width: 223px;">Resistencia en Serie</td>
<td style="width: 68px;"><input align="right" id="Resist" name="Resist" size="7" style="text-align: right;" /></td>
<td style="width: 65px;">[Ohms]</td>
</tr>
<tr>
<td style="width: 223px;"></td>
<td style="width: 68px;"><input align="center" name="Sumbit" onclick="javascript:calculaLeds()" type="button" value="Calcular" /></td>
<td style="width: 65px;"></td>
</tr>
<tr>
<td style="width: 223px;">Valor del Capacitor </td>
<td style="width: 68px;"><input align="right" id="Capacit" name="Capacit" size="7" style="text-align: right;" /></td>
<td style="width: 65px;">[uF]</td>
</tr>
</tbody>
</table>
<br />
</body></html>