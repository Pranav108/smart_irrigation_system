package com.example.myapplication;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.animation.ArgbEvaluator;
import android.animation.ValueAnimator;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {
    private DatabaseReference mDatabase;
    private int nodeA = 0;
    private int nodeB = 0;
    private int main = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Get reference to the Firebase database
        mDatabase = FirebaseDatabase.getInstance().getReference();

        // Add a ValueEventListener to monitor changes in the water values
        ValueEventListener listener = new ValueEventListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                String nodeId = dataSnapshot.getKey();
                for (DataSnapshot nodeSnapshot : dataSnapshot.getChildren()) {
                    int waterValue = nodeSnapshot.child("W").getValue(Integer.class);
                    if (nodeId.equals("A")) {
                        nodeA = waterValue;
                    } else {
                        nodeB = waterValue;
                    }
                    updateUI(nodeId, waterValue);
                }
                int pres = 0;
                if (nodeA == 1 || nodeB == 1) {
                    pres = 1;
                }
                if (pres != main) {
                    main = pres;
                    if (main == 1) {
                        Toast.makeText(MainActivity.this, "Main motor is on", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(MainActivity.this, "Main motor is off", Toast.LENGTH_SHORT).show();
                    }
                }

            }

            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {
                Log.w("error", "onCancelled", databaseError.toException());
            }
        };

        mDatabase.child("A").addValueEventListener(listener);
        mDatabase.child("B").addValueEventListener(listener);
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    private void updateUI(String nodeId, int waterValue) {
        LinearLayout layout;
        TextView textView;

        if (nodeId.startsWith("A")) {
            layout = findViewById(R.id.layout_a);
            textView = findViewById(R.id.text_a);
        } else if (nodeId.startsWith("B")) {
            layout = findViewById(R.id.layout_b);
            textView = findViewById(R.id.text_b);
        } else {
            return; // Invalid node ID
        }
        if (waterValue == 1) {
            textView.setText("Water is needed");
        } else {
            textView.setText("Water requirement fulfilled");
        }

        int startColor = getColor(R.color.light_green);
        int endColor = getColor(R.color.light_red);
        int currentColor = layout.getSolidColor();

        if (waterValue == 1 && currentColor != endColor) {
            animateBackgroundColor(layout, currentColor, endColor);
        } else if (waterValue == 0 && currentColor != startColor) {
            animateBackgroundColor(layout, currentColor, startColor);
        }
    }

    private void animateBackgroundColor(final LinearLayout layout, int startColor, int endColor) {
        ValueAnimator colorAnimator = ValueAnimator.ofObject(new ArgbEvaluator(), startColor, endColor);
        colorAnimator.setDuration(1000); // 1 second duration
        colorAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animator) {
                int animatedColor = (int) animator.getAnimatedValue();
                layout.setBackgroundColor(animatedColor);
            }
        });
        colorAnimator.start();
    }
}