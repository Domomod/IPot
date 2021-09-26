// requirements
var gulp = require('gulp');
var babel = require('gulp-babel');
var del = require('del');
var size = require('gulp-size');

gulp.task('transform', function () {
    var stream = gulp.src('./static/scripts/jsx/*.js')
    .pipe(babel({
        plugins: ['transform-react-jsx']
    }))
      .pipe(gulp.dest('./static/scripts/js/'))
      .pipe(size());
    return stream;
  });

gulp.task('del', function () {
    return del(['./static/scripts/js']);
  });

// tasks
gulp.task('default', gulp.series('del', 'transform', function () {
    gulp.watch('./static/scripts/jsx/*.js', gulp.series('transform'));
}));