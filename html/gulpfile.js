var gulp = require('gulp');
var preprocess = require('gulp-preprocess');
var runSequence = require('run-sequence');
var inline = require('gulp-inline');
var cleancss = require('gulp-clean-css');
var uglify = require('gulp-uglify');
var htmlmin = require('gulp-htmlmin');
var gzip = require('gulp-gzip');
var del = require('del');

/* Clean destination folder */
gulp.task('clean', function() {
    return del(['dist/*']);
});

/* Copy static file */
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
/* preprocess file, inline it, mini - do the same for both variants*/
gulp.task('preproc_seven', function() {
  return gulp.src(['src/index.html'])
  .pipe(preprocess({context: { VARIANT: 'seven', VARIANT_NAME: 'Big Seven', VARIANT_navbar: 'inverse'}}))
  .pipe(inline({
    base: 'src/',
    js: uglify,
    css: cleancss,
  }))
  .pipe(htmlmin({
    collapseWhitespace: true,
    removeComments: true,
    minifyCSS: true,
    minifyJS: true
  }))
  .pipe(gulp.dest('dist'))
});

gulp.task('preproc_nixie', function() {
  return gulp.src(['src/index.html'])
  .pipe(preprocess({context: { VARIANT: 'nixie', VARIANT_NAME:'NAKED Nixie', VARIANT_navbar: 'default'}}))
  .pipe(inline({
    base: 'src/',
    js: uglify,
    css: cleancss,
  }))
  .pipe(htmlmin({
    collapseWhitespace: true,
    removeComments: true,
    minifyCSS: true,
    minifyJS: true
  }))
  .pipe(gulp.dest('dist'))
});
/* GZIPPPP */
gulp.task('gzip', function() {
  return gulp.src(['dist/index.html'])
  .pipe(gzip())
  .pipe(gulp.dest('dist'))
});

/* Some tasks */
gulp.task('seven', function (e) {
   runSequence('clean', 'files_seven', 'preproc_seven', e);
});
gulp.task('seven_fs', function (e) {
   runSequence('clean', 'files_seven', 'preproc_seven', 'gzip', e);
});
gulp.task('nixie', function (e) {
  runSequence ('clean', 'files_nixie', 'preproc_nixie', 'gzip', e);
});
gulp.task('nixie_fs', function (e) {
  runSequence ('clean', 'files_nixie', 'preproc_nixie', 'gzip', e);
});

