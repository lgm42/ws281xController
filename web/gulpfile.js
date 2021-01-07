//https://www.freecodecamp.org/news/super-simple-gulp-tutorial-for-beginners-45141974bfe8/

var gulp = require('gulp'); 
var htmlmin = require('gulp-htmlmin');
var inject = require('gulp-inject');
var gzip = require('gulp-gzip');
var favicon = require('gulp-base64-favicon');
var inline = require('gulp-inline')
var uglify = require('gulp-uglify')
var minifyCss = require('gulp-minify-css')
const fs = require('fs');
var inlineImages = require('gulp-inline-image-html');
var replace = require('gulp-replace');

gulp.task('inline-html', function () {
    return gulp.src('src/index.html')
        //replace dev_env.js by prod_env.js
        .pipe(replace(/dev_env\.js/g, function(match) {
            return 'prod_env.js';
        }))
        .pipe(inline({
            base: 'src/',
            css: minifyCss
          }))
        .pipe(inlineImages('src'))
        .pipe(favicon('src'))
        .pipe(htmlmin({ collapseWhitespace: true, removeComments: true, minifyCSS: true, minifyJS: true}))
        .pipe(gulp.dest('dist'));
});

gulp.task('compress', async function() {
    gulp.src('dist/index.html')
    .pipe(gzip())
    .pipe(gulp.dest('dist'));
});

gulp.task('generate-code', async function() {

    var source = 'dist/index.html.gz';
    var destination = 'dist/index.html.gz.h';

    var wstream = fs.createWriteStream(destination);
    wstream.on('error', function (err) {
        console.log(err);
    });

    var data = fs.readFileSync(source);
 
    wstream.write('#define index_html_gz_len ' + data.length + '\n');
    wstream.write('const char index_html_gz[] PROGMEM = {')

    for (i=0; i<data.length; ++i) 
    {
        if (i % 1000 == 0) 
            wstream.write("\n");
        wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
        if (i < data.length-1) 
            wstream.write(',');
    }

    wstream.write('\n};')
    wstream.end();
});

//gulp.task('watch', function () {
//    gulp.watch('src/**/*', gulp.series('inline-html', 'compress', 'generate-code'));
//});

gulp.task('default', gulp.series('inline-html', 'compress', 'generate-code'));//, 'watch'));