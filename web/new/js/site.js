$(document).ready(function() {
	var reader = new FileReader();
	reader.addEventListener('load', function() {
		$('.image-view').attr('src', reader.result);
		$('.image-original').attr('src', reader.result);
	}, false);
	$('.image-upload').on('change', function() {
		reader.readAsDataURL(this.files[0]);
	});
	$('.filter-setting>input').on('input', function() {
		$(this).siblings('.filter-value').html($(this).val());
	});
	$('.filter-setting>input').on('change', function() {
		var filter = $(this).attr('data-filter');
		var value = $(this).val();
		var clone = $('.image-original').clone();
		clone.removeClass('image-original').addClass('image-view');
		$('.image-view').replaceWith(clone);
		Caman('.image-view', function() {
			this[filter](value).render();
		});
	});
});
