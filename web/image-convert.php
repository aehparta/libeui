<?php
include('config.php');

/* detect errors */
if (!isset($_FILES['image'])) {
    header('Location: '.$siteurl.'/image.php');
    exit;
}
if ($_FILES['image']['error'] !== 0) {
    header('Location: '.$siteurl.'/image.php');
    exit;
}
$type = $_FILES['image']['type'];
$accept = array('image/jpeg', 'image/png', 'image/gif');
if (!in_array($type, $accept)) {
    header('Location: '.$siteurl.'/image.php');
    exit;
}

$filename = $_FILES['image']['tmp_name'];

/* load image */
$image = null;
switch ($type) {
case 'image/jpeg':
    $image = @imagecreatefromjpeg($filename);
    break;
case 'image/png':
    $image = @imagecreatefrompng($filename);
    break;
case 'image/gif':
    $image = @imagecreatefromgif($filename);
    break;
}
if (!$image) {
    header('Location: '.$siteurl.'/image.php');
    exit;
}
@imagepalettetotruecolor($image);
$w_orig = imagesx($image);
$h_orig = imagesy($image);
$w = intval($_POST['width']);
$h = intval($_POST['height']);
$bpp = intval($_POST['bpp']);
$format = $_POST['format'];
$threshold = intval($_POST['threshold']);

/* resize image if so requested */
if ($w != $w_orig && $h != $h_orig && $w > 0 && $h > 0) {
    $image_resized = imagecreatetruecolor($w, $h);
    imagecopyresized($image_resized, $image, 0, 0, 0, 0, $w, $h, $w_orig, $h_orig);
    $image = $image_resized;
}
$w = imagesx($image);
$h = imagesy($image);

/* show page header */
include('header.php');


if ($w != $w_orig || $h != $h_orig) {
    echo "<p>Original image size<ul><li>width: $w_orig</li><li>height: $h_orig</li></ul></p>";
}
echo "<p><h3>Image size</h3><ul><li>width: $w</li><li>height: $h</li><li>bytes with given bpp without compression: ".(($w * $h) / (8 / $bpp))."</li></ul></p>";

echo "<p class='progress'>";
ob_end_flush();

/* auto threshold */
if ($threshold < 1 && $bpp < 8) {
    $low = 255;
    $high = 0;
    for ($y = 0; $y < $h; $y++) {
        for ($x = 0; $x < $w; $x++) {
            $rgb = imagecolorat($image, $x, $y);
            $grey = intval(((($rgb >> 16) & 0xff) + (($rgb >> 8) & 0xff) + ($rgb & 0xff)) / 3);
            if ($grey > $high) {
                $high = $grey;
            }
            if ($grey < $low) {
                $low = $grey;
            }
        }
    }
    $threshold = ($high + $low) / 2;
}

/* convert image data to given bpp */
$cdata = array();
$bit = 0x80;
$byte = 0x00;
for ($y = 0; $y < $h; $y++) {
    for ($x = 0; $x < $w; $x++) {
        $rgb = imagecolorat($image, $x, $y);
        
        if ($bpp == 1) {
            /* monochrome */
            $grey = intval(((($rgb >> 16) & 0xff) + (($rgb >> 8) & 0xff) + ($rgb & 0xff)) / 3);
            if ($grey > $threshold) {
                $byte |= $bit;
            }
            $bit = $bit >> 1;
            if ($bit == 0) {
                $cdata[] = $byte;
                $bit = 0x80;
                $byte = 0x00;
            }
        } else if ($bpp == 2) {
            /* grey 2-bits per pixel */
            $grey = intval(((($rgb >> 16) & 0xff) + (($rgb >> 8) & 0xff) + ($rgb & 0xff)) / 3);
            $th_top = $threshold + ((0xff - $threshold) / 2);
            $th_bottom = $threshold / 2;
            if ($grey > $th_top) {
                $byte |= $bit | ($bit >> 1);
            } else if ($grey > $threshold) {
                $byte |= $bit;
            } else if ($grey > $th_bottom) {
                $byte |= ($bit >> 1);
            }
            $bit = $bit >> 2;
            if ($bit == 0) {
                $cdata[] = $byte;
                $bit = 0x80;
                $byte = 0x00;
            }
        } else if ($bpp == 4) {
            /* grey 4-bits per pixel */
            $grey = intval(((($rgb >> 16) & 0xff) + (($rgb >> 8) & 0xff) + ($rgb & 0xff)) / 3);
            $thresholds = array();
            $thresholds[0x0] = $threshold / 8;
            $thresholds[0x1] = $threshold / 4;
            $thresholds[0x2] = $threshold * 3 / 8;
            $thresholds[0x3] = $threshold / 2;
            $thresholds[0x4] = $threshold * 5 / 8;
            $thresholds[0x5] = $threshold * 6 / 8;
            $thresholds[0x6] = $threshold * 7 / 8;
            $thresholds[0x7] = $threshold;
            $thresholds[0x8] = $threshold + ((0xff - $threshold) / 8);
            $thresholds[0x9] = $threshold + ((0xff - $threshold) / 4);
            $thresholds[0xa] = $threshold + ((0xff - $threshold) * 3 / 8);
            $thresholds[0xb] = $threshold + ((0xff - $threshold) / 2);
            $thresholds[0xc] = $threshold + ((0xff - $threshold) * 5 / 8);
            $thresholds[0xd] = $threshold + ((0xff - $threshold) * 6 / 8);
            $thresholds[0xe] = $threshold + ((0xff - $threshold) * 7 / 8);
            $thresholds[0xf] = 256;
            foreach ($thresholds as $c => $th) {
                if ($grey < $th) {
                    if ($bit == 0x80) {
                        $byte |= $c << 4;
                    } else {
                        $byte |= $c;
                    }
                    break;
                }
            }
            $bit = $bit >> 4;
            if ($bit == 0) {
                $cdata[] = $byte;
                $bit = 0x80;
                $byte = 0x00;
            }
        } else if ($bpp == 8) {
            /* 8-bits per pixel 'rgb' */
            $byte = ((($rgb >> 16) & 0xe0) + (($rgb >> 11) & 0x1c) + (($rgb >> 6) & 0x03));
            $cdata[] = $byte;
        }
    }
}
if ($bpp < 8 && $bit != 0x80) {
    $cdata[] = $byte;
}
ob_start();
echo "</p>";

$data = $cdata;
$type = 'EUI_IMAGE_FORMAT_'.$bpp.'BIT';
if ($format == 'auto') {
    /* compress using RLE */
    $rledata = array();
    $rledata['EUI_IMAGE_FORMAT_'.$bpp.'BIT | EUI_IMAGE_FORMAT_RLE7'] = compress_rle($cdata, 7);
    $rledata['EUI_IMAGE_FORMAT_'.$bpp.'BIT | EUI_IMAGE_FORMAT_RLE6'] = compress_rle($cdata, 6);
    $rledata['EUI_IMAGE_FORMAT_'.$bpp.'BIT | EUI_IMAGE_FORMAT_RLE5'] = compress_rle($cdata, 5);
    $rledata['EUI_IMAGE_FORMAT_'.$bpp.'BIT | EUI_IMAGE_FORMAT_RLE4'] = compress_rle($cdata, 4);
    $rledata['EUI_IMAGE_FORMAT_'.$bpp.'BIT | EUI_IMAGE_FORMAT_RLE3'] = compress_rle($cdata, 3);
    $rledata['EUI_IMAGE_FORMAT_'.$bpp.'BIT | EUI_IMAGE_FORMAT_RLE2'] = compress_rle($cdata, 2);

    echo "<p><h3>Sizes with different compressions</h3><ul>";
    foreach ($rledata as $k => $v) {
        echo "<li>$k: ".count($v)." bytes (".number_format(100.0 * count($v) / count($cdata), 1)." %)\n";
        if (count($v) < count($data)) {
            $data = $v;
            $type = $k;
        }
    }
    echo "</ul>";
}

/* make preview as base64 encoded data into html */
$image_preview = imagecreatetruecolor($w, $h);
$y = 0;
$x = 0;
foreach ($cdata as $byte) {
    if ($bpp == 1) {
        /* monochrome */
        for ($i = 7; $i >= 0; $i--) {
            if ($y >= $h) {
                break;
            }
            
            $c = 0x00;
            $bit = 1 << $i;
            if ($byte & $bit) {
                $c = 0xff;
            }
            imagesetpixel($image_preview, $x, $y, imagecolorallocate($image_preview, $c, $c, $c));
            /* next pixel */
            $x++;
            if ($x >= $w) {
                $y++;
                $x = 0;
            }
        }
    } else if ($bpp == 2) {
        /* grey 2-bits per pixel */
        for ($i = 6; $i >= 0; $i -= 2) {
            if ($y >= $h) {
                break;
            }
            
            $c = 0x00;
            switch (($byte >> $i) & 0x3) {
            case 1:
                $c = 85;
                break;
            case 2:
                $c = 127;
                break;
            case 3:
                $c = 255;
                break;
            }
            imagesetpixel($image_preview, $x, $y, imagecolorallocate($image_preview, $c, $c, $c));
            /* next pixel */
            $x++;
            if ($x >= $w) {
                $y++;
                $x = 0;
            }
        }
    } else if ($bpp == 4) {
        /* grey 4-bits per pixel */
        for ($i = 4; $i >= 0; $i -= 4) {
            if ($y >= $h) {
                break;
            }
            
            $c = intval((($byte >> $i) & 0xf) * 17);
            imagesetpixel($image_preview, $x, $y, imagecolorallocate($image_preview, $c, $c, $c));
            /* next pixel */
            $x++;
            if ($x >= $w) {
                $y++;
                $x = 0;
            }
        }
    } else if ($bpp == 8) {
        /* 8-bits per pixel 'rgb' */
        $r = (($byte & 0xe0) >> 5) * 36; /* 3-bits */
        $g = (($byte & 0x1c) >> 2) * 36; /* 3-bits */
        $b = ($byte & 0x03) * 85; /* 2-bits */
        imagesetpixel($image_preview, $x, $y, imagecolorallocate($image_preview, $r, $g, $b));
        /* next pixel */
        $x++;
        if ($x >= $w) {
            $y++;
            $x = 0;
        }
    }
}
$file_preview = tempnam('/tmp', 'libeui.image-preview');
imagepng($image_preview, $file_preview, 9);
$base64_preview = base64_encode(file_get_contents($file_preview));
unlink($file_preview);
echo "<p><img id='image-preview' alt='preview' src='data:image/png;base64,$base64_preview' /></p>";

/* print actual data as C-code */
print_cdata($data, 'Image data', $w, $h, $type);

echo "<p><h3>Definitions</h3><pre class='small-text'>";
echo "typedef struct\n";
echo "{\n";
echo "    uint16_t width;\n";
echo "    uint16_t height;\n";
echo "    uint8_t type;\n";
echo "    uint8_t *data;\n";
echo "} eui_image_t;\n";
echo "\n";
echo "#define IMAGE_FORMAT_1BIT        1\n";
echo "#define IMAGE_FORMAT_2BIT_RLE3   2\n";
echo "#define IMAGE_FORMAT_4BIT_RLE4   4\n";
echo "#define IMAGE_FORMAT_8BIT_RLE5   8\n";
echo "</pre></p>";


include('footer.php');


function print_cdata($data, $title, $w, $h, $type)
{
    echo "<p><h3>$title</h3><pre class='small-text'>";
    echo "const eui_image_data_t image =\n";
    echo "{\n";
    echo "    $w, $h, $type,\n";
    echo "    { /* size of data in bytes: ".count($data)." */\n";
    $break = 0;
    foreach ($data as $byte) {
        if ($break == 0) {
            echo "        ";
        }
        printf("0x%02x, ", $byte);
        $break++;
        if ($break > 15) {
            $break = 0;
            echo "\n";
        }
    }
    if ($break != 0) {
        echo "\n";
    }
    echo "    },\n";
    echo "};\n";
    echo "</pre></p>";
}

function compress_rle($data, $break_bit = 5)
{
    $rledata = array();
    $same = false;
    $count = 0;
    $low = (1 << $break_bit) - 1;
    $high = 0xff - $low;
    foreach ($data as $byte) {
        if ($count < 1) {
            $same = $byte;
            $count = 1;
            continue;
        }
        if ($same != $byte || $count >= $low) {
            if ($count > 2 || $same > $high) {
                $rledata[] = $high | $count;
                $rledata[] = $same;
            } else {
                $rledata[] = $same;
                if ($count > 1) {
                    $rledata[] = $same;
                }
            }
            $same = $byte;
            $count = 1;
        } else {
            $count++;
        }
    }
    /* add rest */
    if ($count > 2 || $same > $high) {
        $rledata[] = $high | $count;
        $rledata[] = $same;
    } else {
        $rledata[] = $same;
        if ($count > 1) {
            $rledata[] = $same;
        }
    }
    
    return $rledata;
}
