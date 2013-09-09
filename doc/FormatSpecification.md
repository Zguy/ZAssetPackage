# Format Specification
## Version 1.0

<table>
<tr><th>Example</th>   <th>Bytes</th> <th>Description</th></tr>
<tr><td colspan="3"><h4>Header</h4></td></tr>
<tr><td>ZA</td>        <td>2</td>     <td>Magic number, always "ZA"</td></tr>
<tr><td>0</td>         <td>1</td>     <td>[Version](#versions)</td></tr>
<tr><td>0</td>         <td>1</td>     <td>[Compression](#compressions)</td></tr>
<tr><td colspan="3"><h4>Lookup table</h4></td></tr>
<tr><td>1</td>         <td>4</td>     <td>Number of entries</td></tr>
<tr><td colspan="3"><h5>Entry</h5></td></tr>
<tr><td>"1.png\0"</td> <td>6</td>     <td>Zero terminated filename</td></tr>
<tr><td>0</td>         <td>4</td>     <td>File index</td></tr>
<tr><td>3</td>         <td>4</td>     <td>Original file size</td></tr>
<tr><td>3</td>         <td>4</td>     <td>Archive file size (after compression)</td></tr>
<tr><td colspan="3"><h4>Data</h4></td></tr>
<tr><td>xxx</td>       <td>3</td>     <td>Data, compressed with the method specified in the header</td></tr>
</table>

<h3 id="versions">Versions</h3>
<table>
<tr><th>Value</th><th>Description</th></tr>
<tr><td>0</td>    <td>Version 1.0</td></tr>
</table>

<h3 id="compressions">Compressions</h3>
<table>
<tr><th>Value</th><th>Description</th></tr>
<tr><td>0</td>    <td>No compression</td></tr>
<tr><td>1</td>    <td>LZ4</td></tr>
</table>
