//https://www.freecodecamp.org/news/super-simple-gulp-tutorial-for-beginners-45141974bfe8/

var gulp = require('gulp'); 
var cssnano = require('gulp-cssnano'); 
var sass = require('gulp-sass'); 
var concat = require('gulp-concat'); 
var uglify = require('gulp-uglify');
const { watch } = require('gulp');

gulp.task('sass', function(){    
    return gulp.src('src/style.scss')       
        .pipe(sass())       
        .pipe(cssnano())       
        .pipe(gulp.dest('dist')); 
});

gulp.task('js', function(){    
    return gulp.src('src/js/*.js')          
        .pipe(concat('all.js'))       
        .pipe(uglify())       
        .pipe(gulp.dest('dist')); 
});

gulp.task('watch', function(){       
	gulp.watch('src/*.scss', gulp.series('sass'));          
    gulp.watch('src/js/**/*.js', gulp.series('js')); 
});

gulp.task('default', gulp.parallel('sass', 'js', 'watch'));