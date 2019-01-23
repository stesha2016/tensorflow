/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

package org.tensorflow.demo.driver;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;

import org.tensorflow.demo.driver.Classifier.Recognition;

import java.util.List;

public class RecognitionScoreView extends View implements ResultsView {
  private static final float TEXT_SIZE_DIP = 24;
  private List<Recognition> results;
  private final float textSizePx;
  private final Paint fgPaint;
  private final Paint bgPaint;

  public RecognitionScoreView(final Context context, final AttributeSet set) {
    super(context, set);

    textSizePx =
        TypedValue.applyDimension(
            TypedValue.COMPLEX_UNIT_DIP, TEXT_SIZE_DIP, getResources().getDisplayMetrics());
    fgPaint = new Paint();
    fgPaint.setTextSize(textSizePx);

    bgPaint = new Paint();
    bgPaint.setColor(0xcc4285f4);
  }

  @Override
  public void setResults(final List<Recognition> results) {
    this.results = results;
    postInvalidate();
  }

  @Override
  public void onDraw(final Canvas canvas) {
    final int x = 10;
    int y = (int) (fgPaint.getTextSize() * 1.5f);

    canvas.drawPaint(bgPaint);

    if (results != null) {
      for (final Recognition recog : results) {

        canvas.drawText(matchTitle(recog.getTitle()) + ": " + recog.getConfidence(), x, y, fgPaint);
        y += fgPaint.getTextSize() * 1.5f;
      }
    }
  }

  private String matchTitle(final String title) {
    switch (title) {
      case "0:c0":
        return "safe driving";
      case "1:c1":
        return "texting-right";
      case "2:c2":
        return "talking phone-right";
      case "3:c3":
        return "texting-left";
      case "4:c4":
        return "talking phone-left";
      case "5:c5":
        return "operating radio";
      case "6:c6":
        return "drinking";
      case "7:c7":
        return "reaching behind";
      case "8:c8":
        return "hair and makeup";
      case "9:c9":
        return "talking to passenger";
    }

    return "unknown";
  }
}
