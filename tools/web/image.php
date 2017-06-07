<?php
include('config.php');
include('header.php');
?>

<p><form method='post' action='<?=$siteurl?>/image-convert.php' enctype='multipart/form-data'>
<div class='site-form'>
<label for='image'>Image to convert (accepts png/jpeg/gif)</label><br />
<input class='site-form-input' type='file' id='image' name='image' /><br />

<label for='bpp'>Bits per pixel in result</label><br />
<select class='site-form-input' id='bpp' name='bpp' />
    <option value='1'>1 (monochrome, 8 pixels per byte)</option>
    <option value='2'>2 (grey, 4 pixels per byte)</option>
    <option value='4'>4 (grey, 2 pixels per byte)</option>
    <option value='8'>8 (1 pixel per byte 'RGB')</option>
    <option value='16'>16 (RGB, 2 bytes per pixel)</option>
    <option value='24'>24 (RGB, 3 bytes per pixel)</option>
    <option value='32'>32 (RGBA, 4 bytes per pixel)</option>
</select><br />

<label for='format'>Format of result</label><br />
<select class='site-form-input' id='format' name='format' />
    <option value='auto'>Auto</option>
    <option value='uncompressed'>Uncompressed</option>
</select><br />

<label>Resize image</label><br />
<label for='width'>width:</label><input class='site-form-input site-form-input-short' type='text' value='' id='width' name='width' />
<label for='height'>height:</label><input class='site-form-input site-form-input-short' type='text' value='' id='height' name='height' />
<br />
<label for='threshold'>Threshold for monochrome/2-bit/4-bit pictures (0-255, default 127)</label><br />
<input class='site-form-input site-form-input-short' type='text' value='' id='threshold' name='threshold' /><br />

<br />
<input class='site-form-button' type='submit' value='Convert' />
</div>
</form></p>

<?php
include('footer.php');
?>
