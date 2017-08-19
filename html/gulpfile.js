var gulp = require('gulp');
var preprocess = require('gulp-preprocess');
var runSequence = require('run-sequence');
var inline = require('gulp-inline');
var cleancss = require('gulp-clean-css');
var uglify = require('gulp-uglify');
var htmlmin = require('gulp-htmlmin');
var gzip = require('gulp-gzip');
var useref = require('gulp-useref');
var gulpif = require('gulp-if');
var del = require('del');

/* Clean dist/build folder */
gulp.task('clean:all', function() {
    return del(['dist/*']);
});
gulp.task('clean:build', function() {
    return del(['dist/build/*']);
});

/* copy static file based on variant */
gulp.task('files_nixie', function() {
  return gulp.src([
    'src/**/glyphicons.woff2',
    'src/**/ubuntu*.woff2',
    'src/**/favicon_nixie.ico',
  ])
  .pipe(gulp.dest('dist'));
});

gulp.task('files_seven', function() {
  return gulp.src([
    'src/**/glyphicons.woff2',
    'src/**/roboto*.woff2',
    'src/**/favicon_seven.ico',
  ])
  .pipe(gulp.dest('dist'));
});
/* Preprocess file, do the same for both variants, merge css and js */
gulp.task('preproc_seven', function() {
  return gulp.src(['src/index.html'])
  .pipe(preprocess({context: { VARIANT: 'seven', VARIANT_NAME: 'Big Seven', VARIANT_navbar: 'inverse'}}))
  .pipe(useref())
  .pipe(gulpif('*.css', cleancss()))
  .pipe(gulpif('*.js', uglify()))
  .pipe(gulp.dest('dist'))
});

gulp.task('preproc_nixie', function() {
  return gulp.src(['src/index.html'])
  .pipe(preprocess({context: { VARIANT: 'nixie', VARIANT_NAME:'NAKED Nixie', VARIANT_navbar: 'default'}}))
  .pipe(useref())
  .pipe(gulpif('*.css', cleancss()))
  .pipe(gulpif('*.js', uglify()))
  .pipe(gulp.dest('dist'))
});

/* build file system for ESP8266, combine all files into one big file and gzip it */
gulp.task('gzip', function() {
  return gulp.src(['dist/index.html'])
  .pipe(inline({
    base: 'dist/',
    js: uglify,
    css: cleancss,
    disabledTypes: ['svg', 'img']
  }))
  .pipe(htmlmin({
    collapseWhitespace: true,
    removeComments: true,
    minifyCSS: true,
    minifyJS: true
  }))
  .pipe(gzip())
  .pipe(gulp.dest('dist/build'))
});

gulp.task('files_fs', function() {
  return gulp.src([
    'dist/**/*.{ico,woff2}'
  ])
  .pipe(gulp.dest('dist/build'));
});

/* tasks */
gulp.task('seven', function (e) {
   runSequence('clean:all', 'files_seven', 'preproc_seven', e);
});
gulp.task('nixie', function (e) {
  runSequence ('clean:all', 'files_nixie', 'preproc_nixie', e);
});
gulp.task('build_fs', function (e) {
   runSequence('clean:build', 'gzip', 'files_fs', e);
});
/* watch files for faster developemnt */
gulp.task('seven_watch', function() {
  gulp.watch('src/index.html', ['seven']);
});

gulp.task('nixie_watch', function() {
  gulp.watch('src/index.html', ['nixie']);
});

